#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>

#define GET 1
#define POST 2
#define DELETE 3

typedef struct location {
	bool 		methodsAllowed[3];
	int			redirectionCode;
	std::string	redirectionPath;
	std::string	root;
	bool		directoryListing;
	std::string	index;
	bool		acceptUploadedFile;
	std::string	uploadedFilePath;
	//add CGI content

} location;

typedef struct server {
	std::string 			address; //address = host:port
	std::string 			server_name;
	bool					isDefaultServer;
	std::string 			errpage;
	int						maxFileSizeUpload;
	std::vector<location>	locations;
} server;

const std::vector<server> parse_conf_file(const std::string path);
const std::string ParsingError(std::string error) throw();

#endif
