<?php

	// Include.
	include("_mysql_config.php");
	
	// Variables.
	$_mysql_connection = false;

	// Connect to the MySQL server as specified in the config and return the connection object.
	// This can be called repeatedly to regain the connection handle if it is lost.
	function MySQL_Start()
	{
		global $_mysql_connection, $_mysql_hostname, $_mysql_username, $_mysql_password;
		
		$_mysql_connection = mysql_connect($_mysql_hostname, $_mysql_username, $_mysql_password);
		return $_mysql_connection;
	}
	
	// Close the connection to the MySQL server.
	function MySQL_Stop()
	{
		global $_mysql_connection;
	
		if ($_mysql_connection)
		{
			mysql_close($_mysql_connection);
			$_mysql_connection = 0;
		}	
	}
	
	// Perform a query using the existing connection on the specified database.
	function MySQL_CreateQuery($database_name, $query)
	{
		global $_mysql_connection;
		
		if ($_mysql_connection)
		{
			if (mysql_select_db($database_name, $_mysql_connection))
			{
				return mysql_query($query, $_mysql_connection);
			}
		}
		
		return false;
	}
	
	// Free the MySQL query result.
	// Once the data has been retrieved, this can be executed without fear of losing any data.
	function MySQL_FreeQuery($query_result)
	{
		mysql_free_result($query_result);
	}
	
	// Get the number of rows returned from the query.
	function MySQL_GetNumRows($query_result)
	{
		global $_mysql_connection;
		
		if ($_mysql_connection)
		{
			return mysql_num_rows($query_result);
		}
		
		return false;
	}
	
	// Fetch the data for the next result row from the query.
	function MySQL_FetchRow($query_result)
	{
		global $_mysql_connection;
		
		if ($_mysql_connection)
		{
			return mysql_fetch_assoc($query_result);
		}
		
		return false;
	}

?>