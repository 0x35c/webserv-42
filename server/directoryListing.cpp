#include "Request.hpp"

void Request::directoryListing(DIR* directory) {
	std::string tmpHTML = "<!DOCTYPE html>\
<html lang=\"en\">\
<head>\
	<link rel=\"icon\" href=\"server/images.png\">\
</head>\
<body>\
<p>\
<ol>\
";

	struct dirent* file;
	while (true) {	
		file = readdir(directory);
		if (file == NULL)
			break;
		tmpHTML += "<li><a href=\"";
		tmpHTML += file->d_name;
		tmpHTML += "\">";
		tmpHTML += file->d_name;
		tmpHTML += "</a></li>\n";
	}
	tmpHTML += "</ol>\
	</p>\
	</body>\
	</html>";
	size_t fileSize = tmpHTML.length();

	std::ostringstream ss;
	ss << "HTTP/1.1 " << _statusCode << "\r\n";
	ss << "Content-type: " + _requestHeader[ACCEPT] + "\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";
	ss << tmpHTML;
	write(_clientfd, ss.str().c_str(), ss.str().size());
	ss.str("");
	ss.clear();
	closedir(directory);
}
