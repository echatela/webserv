#include "conn_handler.hpp"
#include "cgi_handler.hpp"
#include "cgi_in_handler.hpp"
#include "event_handler.hpp"
#include "epoll.hpp"
#include "http_response.hpp"
#include "listen_handler.hpp"
#include "reactor.hpp"
#include "../webserv.hpp"
#include "router.hpp"

#include <cstddef>
#include <ctime>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <exception>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>

#define RESET   "\033[0m"
#define YELLOW  "\033[33m"      /* Yellow */
#define CYAN    "\033[36m"      /* Cyan */

static void	CloseFd(int & fd) {
	if (fd >= 0) {
		close(fd);
		fd = -1;
	}
}

ConnHandler::ConnHandler(sockaddr_in addr, int fd, const ListenHandler & listen,
	 Epoll & epoll, Reactor & reactor)
	: fd_(fd), state_(kReading), addr_(addr), write_off_(0)
	, start_time_(time(NULL)), epoll_(epoll), reactor_(reactor)
	, listen_(listen), cgi_(NULL)
{
	try {
		router_.set_config(const_cast<Config &>(listen_.config()));
		parser_.set_max_body_size(listen_.config().max_body_size());
		epoll_.Add(fd_, EPOLLIN, this);
	} catch (std::exception &) {
		close(fd_);
		throw;
	}
}

int	ConnHandler::HandleEvent(uint32_t events)
{
	if (events & (EPOLLERR | EPOLLHUP))
		return kClose;

	if (state_ == kCgi)
		return kKeep;

	if (CheckTimeout(time(NULL)) == kClose)
		return kClose;

	if (state_ == kReading && events & EPOLLIN) {
		char read_buf[kReadBufferSize];

		ssize_t n = recv(fd_, read_buf, kReadBufferSize - 1, 0);
		if (n <= 0)
			return kClose;
		read_buf[n] = '\0';

		if (parser_.Add(read_buf, n))
			HandleRequest();
		return kKeep;
		
	} else if (state_ == kWriting && events & EPOLLOUT) {
		size_t	remaining = write_buf_.size() - write_off_;
		ssize_t	n = send(
			fd_, &write_buf_[0] + write_off_, remaining, 0);

		if (n > 0)
			write_off_ += n;
		// decide to respect http/1.0 protocol here for simplicity
		// and security
		if (write_off_ == write_buf_.size())
			return kClose;
		return kKeep;
	}
	return kClose;
}

void	ConnHandler::SendResponse(HttpResponse& response) {

	response.set_header("Connection", "close");
	write_buf_ = response.ToCharVector();
	state_ = kWriting;
	epoll_.Mod(fd_, EPOLLOUT, this);
}

void	ConnHandler::HandleRequest()
{
	if (parser_.bad_request()) {
		HttpResponse	resp = Router::ErrorResponse(kBadRequest);
		SendResponse(resp);
		return;
	}
	if (parser_.too_large()) {
		HttpResponse	resp = Router::ErrorResponse(kPayloadTooLarge);
		SendResponse(resp);
		return;
	}

	parser_.ParseRequest(request_);
	
	RouteResult result = router_.ProcessRequest(request_);
	switch (result.type()) {
		case kRouteResponse: {
			HttpResponse resp = result.response();
			SendResponse(resp);
			break;
		}
		case kRouteCgi:
			StartCgi(result.plan());
			break;
		default:
			break;
	}
}



std::vector<std::string> ConnHandler::BuildCgiEnv(const CgiPlan & plan) const
{
	std::vector<std::string>	env;
	env.push_back("AUTH_TYPE="); /* AUTH_TYPE don't need to be defined
	because we don't do authentification */
	std::stringstream content_length;
	content_length << "CONTENT_LENGTH=" << request_.body().size();
	env.push_back(content_length.str());
	std::stringstream content_type;
	content_type << "CONTENT_TYPE=" << request_.header("content-type");
	env.push_back(content_type.str());
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("PATH_INFO=" + plan.path_info);
	env.push_back("PATH_TRANSLATED=");
	env.push_back("QUERY_STRING=" + plan.query_string);
	env.push_back("REMOTE_ADDR=" + webserv::utils::AddrToString(addr_));
	env.push_back("REMOTE_HOST=" + webserv::utils::AddrToString(addr_));
	env.push_back("REMOTE_IDENT="); // authentification, no need to include
	env.push_back("REMOTE_USER="); // authentification, no need to include
	env.push_back("REQUEST_METHOD=" + request_.method()); // a changer selon la methode
	env.push_back("SCRIPT_NAME=" + plan.script_name);
	std::string	server_name = listen_.config().server_name();
	// Value "Host" in the request
	if (server_name.empty())
		server_name = listen_.listen_info().host;
	env.push_back("SERVER_NAME=" + server_name);
	env.push_back("SERVER_PORT=" + listen_.listen_info().port);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.0");

	return env;
}

void	ConnHandler::StartCgi(const CgiPlan & plan) {
	
	std::string			path;
	std::string			script_dir;
	std::vector<std::string>	argv_str;
	std::vector<std::string>	envp_str;
	int				out_pipe[2] = {-1, -1};
	int				in_pipe[2] = {-1, -1};

	state_ = kCgi;

	script_dir = webserv::utils::Basedir(plan.script_path);

	if (plan.interpreter.empty())
		path = "./" + webserv::utils::Basename(plan.script_path);
	else
		path = plan.interpreter;

	argv_str.push_back(webserv::utils::Basename(path));
	if (path == plan.interpreter)
		argv_str.push_back(webserv::utils::Basename(plan.script_path));

	envp_str = BuildCgiEnv(plan);

	try {
		if (pipe(out_pipe) < 0)
			throw std::runtime_error("pipe() failed");
		if (webserv::fd::SetNonBlock(out_pipe[0]) < 0
			|| webserv::fd::SetCloExec(out_pipe[0]) < 0)
			throw std::runtime_error("SetNonBlock() failed");

		if (pipe(in_pipe) < 0)
			throw std::runtime_error("pipe() failed");
		if (webserv::fd::SetNonBlock(in_pipe[1]) < 0
			|| webserv::fd::SetCloExec(in_pipe[1]) < 0)
			throw std::runtime_error("SetNonBlock() failed");

		std::vector<char*> argv = webserv::utils::ToCStrArray(argv_str);
		std::vector<char*> envp = webserv::utils::ToCStrArray(envp_str);

		pid_t	pid = fork();
		if (pid < 0)
			throw std::runtime_error("fork() failed");

		if (pid == 0) {
			if (dup2(in_pipe[0], STDIN_FILENO) < 0)
				_exit(1);
			if (dup2(out_pipe[1], STDOUT_FILENO) < 0)
				_exit(1);
			close(in_pipe[0]); close(in_pipe[1]);
			close(out_pipe[0]); close(out_pipe[1]);
			if (chdir(script_dir.c_str()) < 0)
				_exit(1);
			execve(path.c_str(), &argv[0], &envp[0]);
			_exit(1);
		}

		CloseFd(in_pipe[0]);
		CloseFd(out_pipe[1]);

		if (request_.body().empty())
			CloseFd(in_pipe[1]);
		else {
			CgiInHandler *cgi_in = new CgiInHandler(
				in_pipe[1], epoll_, request_.body());
			in_pipe[1] = -1;
			reactor_.AddEventHandler(cgi_in);
		}
		cgi_ = new CgiHandler(pid, out_pipe[0], *this, epoll_);
		out_pipe[0] = -1;
		reactor_.AddEventHandler(cgi_);
	} catch (std::exception&) {
		CloseFd(in_pipe[0]);
		CloseFd(in_pipe[1]);
		CloseFd(out_pipe[0]);
		CloseFd(out_pipe[1]);
		throw;
	}
}

void	ConnHandler::OnCgiDone(const std::string & output)
{
	HttpResponse	response = router_.CgiResponse(output);

	cgi_ = NULL;
	write_buf_ = response.ToCharVector();
	epoll_.Mod(fd_, EPOLLOUT, this);
	state_ = kWriting;
}

void	ConnHandler::OnCgiError(int status)
{
	HttpResponse	response(router_.ErrorResponse(status));

	cgi_ = NULL;
	write_buf_ = response.ToCharVector();
	epoll_.Mod(fd_, EPOLLOUT, this);
	state_ = kWriting;
}

void	ConnHandler::Detach() { cgi_ = NULL; }

int	ConnHandler::CheckTimeout(time_t now)
{
	if (difftime(now, start_time_) >= kTimeoutSecs)
		return kClose;
	return kKeep;
}

ConnHandler::~ConnHandler()
{
	if (cgi_)
		cgi_->Detach();
	try {
		epoll_.Del(fd_);
	} catch (std::exception &) {}
	close(fd_);
}
