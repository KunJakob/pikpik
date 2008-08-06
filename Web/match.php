<?php

	// Includes.
	include("_mysql.php");
	include("_stream.php");
	include("_constant.php");
	include("_result.php");
	
	// Establish a connection with the MySQL database.
	SQL_Connect();
	
	// Initialise shared variables.
	$mysql_database 	= "matchservice";
	$current_time		= time();
	
	// Read in the stream values.
	$stream_values = $_POST;
	
	if (!$stream_values)
	{
		include("_match_public.php");
		
		SQL_Disconnect();
		die;
	}
	
	// Verify from application.
	// "REQUEST_METHOD" == "POST"
	// "HTTP_USER_AGENT" == "SMM-1"
	
	// Process the command.
	if (isset($_GET["list"]))
	{
		include("_match_list.php");
	}
	elseif (isset($_GET["ping"]))
	{
		include("_match_ping.php");
	}
	elseif (isset($_GET["create"]))
	{
		include("_match_create.php");
	}
	elseif (isset($_GET["update"]))
	{
		include("_match_update.php");
	}
	elseif (isset($_GET["close"]))
	{
		include("_match_close.php");
	}
	
	// Close the MySQL connection.
	SQL_Disconnect();
	
?>