<?php

	// Gererate a unique password for the new session.
	function GeneratePassword()
	{
		$password_length = 12;
		$chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		
		$password = "";		

		for ($i = 0; $i < $password_length; $i++)
		{
			$password .= $chars[rand(0, strlen($chars) - 1)];
		}
		
		return $password;
	}

	// Initialise variables for the create operation.
	$timestamp = time();
	
	// Verify from application.
	// "REMOTE_ADDR" == "IP"
	// "REQUEST_METHOD" == "POST"
	// "HTTP_USER_AGENT" == "SAPIAN-MM"
	
	// Verify all input data.

	// Create the new session.
	/*$query_create = MySQL_CreateQuery
	(
		$mysql_database, 
		"INSERT INTO sessions".
		"VALUES ()"
	);*/
	
	$debug_file = fopen("lastquery.txt", "w");
	fwrite($debug_file, Stream_ReadBuffer());
	fclose($debug_file);

?>