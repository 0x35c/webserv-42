#include "Request.hpp"

void Request::directoryListing(DIR* directory, const std::string& dirName) {
	std::string tmpHTML = "<!DOCTYPE html>\
	<html lang=\"en\">\
	<head>\
		<meta charset=\"utf-8\">\
	</head>\
	<body style=\"background-color: rgb(181, 200, 95);\
	font-family: \"sans-serif\";\">\
	<h1>You are on " + dirName + " directory</h1>\
	<h1><a href=\"/index.html\">HOME</a></br></h1>\
	<p>\
	<ul>";
	(void)dirName;

	struct dirent* file;
	while (true) {	
		file = readdir(directory);
		if (file == NULL)
			break;
		if (file->d_name[0] == '.')
			continue ;
		tmpHTML += "<li><span><a href=\"";
		tmpHTML += file->d_name;
		tmpHTML += "\">";
		tmpHTML += file->d_name;
		tmpHTML += "</a></span></li>\n";
	}
	tmpHTML += "</ul>\
	</p>\
	</body>\
	</html>";
	size_t fileSize = tmpHTML.length();

#if DEBUG
	std::cout << "Status code: " << _statusCode << std::endl;
#endif
	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	if (_requestHeader[LOCATION] != "")
		ss << "Location: " << _requestHeader[LOCATION] << "\r\n";
	ss << "Content-type: " + _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";
	ss << tmpHTML;
	write(_clientfd, ss.str().c_str(), ss.str().size());
	ss.str("");
	ss.clear();
	closedir(directory);
}
