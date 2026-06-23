#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <vector>
#include "http_parser.hpp"

enum	HttpStatusCode {
	kOk = 200,
	kCreated = 201,
	kNoContent = 204,
	kBadRequest = 400,
	kForbidden = 403,
	kNotFound = 404,
	kMethodNotAllowed = 405,
	kPayloadTooLarge = 406,
	kInternalServerError = 500
};

struct	Header {
	std::string	key;
	std::string	value;
};

class HttpResponse {

public:
	HttpResponse();
	HttpResponse(const HttpResponse &src);
	HttpResponse &operator=(const HttpResponse &rhs);
	~HttpResponse();

	void			set_version(std::string version);
	void			set_status(int status);
	void			set_header(std::string key, std::string content);
	void			set_body(std::string body_content);
	void			set_reason_phrase();

	std::string		get_body() const;

	std::string			ToString();
	std::vector<char>	ToCharVector();

	static std::string get_reason_phrase(int status_code);

private:
// type
	int			status_code_;
	std::string		reason_phrase_;
	std::string		body_;
	std::vector<Header>	headers_;
	std::string		version_;

};

#endif


// POURQUOI PRAGMA ONCE -> 

// ici enum de status code mais peut etre focntion generale suffit avec codes stockes en dur dedans

// generer Content-length toujours (taille du body)

// stocker les extensions, map de content_Type a initialiser
	// .html -> text/htmlRequest
	// .css  -> text/css
	// .js   -> application/javascript
	// .png  -> image/png
	// .jpg  -> image/jpeg
	// .gif  -> image/gif

// Fonction pour construire les pages erreur

// GET
	// trouver le fichier demande
		// root + url
		// stat() -> verifier son existence

	// verifier aue e chemin reste dans le root configure

	// verifier les permissions


// requete recue 
// requete parsee
// requete executee
// requete envoyee ? ou l'inverse -> a mediter ...


// socket acceptée
//     ↓
// lecture de la requête
//     ↓
// parsing HTTP
//     ↓
// construction d'un objet Request
//     ↓
// traitement (route, méthode, CGI, upload...) -> verifs ? DELETE  
//     ↓
// construction d'un objet Response
//     ↓
// sérialisation HTTP
//     ↓
// send()


// question ouverte : a quoi sert la reponse si le boulot a ete fait avant ?
						// garder une trace ?
						// ca se tient lol
