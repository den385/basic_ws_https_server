#include "webserver/stdafx.h"

#include "webserver/connection.h"



namespace net
{
	template<>
	void Connection<TCPSocket>::_cancel()
	{
		error_code ec;
		
		sock_->cancel(ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while socket cancel", ec);
	}
	
	template<>
	void Connection<SSLSocket>::_cancel()
	{
		error_code ec;
		
		sock_->lowest_layer().cancel(ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while socket cancel", ec);
	}
	
	template<>
	void Connection<TCPSocket>::_shutdown()
	{
		error_code ec;
		
		sock_->shutdown(sock_->shutdown_both, ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while socket shutdown", ec);
	}
	
	template<>
	void Connection<SSLSocket>::_shutdown()
	{
		error_code ec;
		
		sock_->lowest_layer().shutdown(sock_->lowest_layer().shutdown_both, ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while socket shutdown", ec);
	}
	
	template<>
	void Connection<TCPSocket>::_close()
	{
		error_code ec;
		
		sock_->close(ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while socket close", ec);
	}
	
	template<>
	void Connection<SSLSocket>::_close()
	{
		error_code ec;
		
		sock_->lowest_layer().close(ec);
		
		if (ec)
			IFLOG(P2, "Exception happened while socket close", ec);
	}
	
	
	
	template class Connection<SSLSocket>;
	
	template class Connection<TCPSocket>;
}