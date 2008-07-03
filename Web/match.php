<?php

	// Includes.
	include("_mysql.php");
	include("_stream.php");
	include("_constant.php");
	
	// Initialise shared variables.
	$mysql_database = "matchservice";
	
	// Read in the stream values.
	$stream_values = Stream_GenerateAssocArray();
	
	if (!$stream_values)
	{
		// Show the public interface.
		//include("_match_public.php");
	}
	
	// Establish a connection with the MySQL database.
	MySQL_Start();
	
	// Process the command.
	if (isset($_GET["list"]))
	{
		// List all the active sessions.
		//include("_match_list.php");
	}
	elseif (isset($_GET["ping"]))
	{
		// Reset the expiry for the session.
		//include("_match_ping.php");
	}
	elseif (isset($_GET["create"]))
	{
		// Create a new session, a password will be returned.
		include("_match_create.php");
	}
	elseif (isset($_GET["update"]))
	{
		// Update the session details.
		//include("_match_update.php");
	}
	elseif (isset($_GET["destroy"]))
	{
		// Close the session cleanly.
		//include("_match_destroy.php");
	}
	else
	{
		// Show the public interface.
		//include("_match_public.php");
	}
	
	// Close the MySQL connection.
	MySQL_Stop();
	
?>