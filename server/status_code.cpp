#include "Server.hpp"

// Here is a short summary of the most common status codes
//
// 200 Status codes represent successful responses
//
// 200 - OK => Generic success status (the request 
// was successful, whatever it was trying to do)
//
// 201 - Created => Same as 200 but specific to POST method
//
// 204 - No content => Same as 200 but specific to DELETE method
//
//
// 300 Status codes are for redirecting
// 
// 304 - Not Modified => Status code used for caching,
// when the requested resource has not been changed
// and it is still available in the cache
//
//
// 400 Status codes are for errors due to client input
//
// 400 - Bad request => Generic bad request (incorrect data)
//
// 403 - Forbidden => Informs the client it does not have
// the permission for this request
//
// 404 - Not Found => The resource couldnt be found
// (for example when you try to access something that
// does not exist)
//
// 429 - Too Many Requests => Must send a Retry-After attribute
// in the HTTP header with the amount of time before retry
//
//
// 500 Status codes are for errors due to the server
//
// 500 - Internal Server Error => Used in any situation when
// the server has an error but does not have a specific code 
// to handle it
//
// 503 - Service Unavailable => The server cannot handle the
// request, must send a Retry-After attribute too

bool Server::setStatusCode(void) {
	std::ifstream file;

	file.open(_requestHeader["PATH"].c_str());
	if (!file) {
		_statusCode = "404 Not Found";
		return (false);
	}
	else
		_statusCode = "200 OK";
	return (true);
}