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

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$gid		= $stream_values["gid"];
	$time		= $current_time;
	$expiry		= $current_time + $SESSION_TIMEOUT;
	$owner		= $_SERVER["REMOTE_ADDR"];
	$sid		= $stream_values["sid"];
	$pass		= GeneratePassword();
	$ip			= $_SERVER["REMOTE_ADDR"];
	$state		= $STATUS_CREATING;
	$title		= $stream_values["title"];
	$tslots		= $stream_values["slots"];
	$uslots		= 0;
	$players	= "";
	$info		= $stream_values["info"];
	
	// ==================================================
	// Verify all input data.
	// ==================================================

	// Verify that the session id is valid.
	
	// Verify that the title is valid.
	
	// Verify that the slots are valid.
	
	// Verify that the info is valid.

	// ==================================================
	// Check the credentials of the session.
	// ==================================================
	
	// Check that an active session from this IP does not already exist.
	$query = SQL_Query
	(
		$mysql_database,
		"SELECT * FROM sessions WHERE owner = '$owner' AND expiry > $time LIMIT 1"
	);
	
	if (SQL_GetNumRows($query))
		Result_Error($ERROR_OWNER_EXISTS);
	
	SQL_FreeQueryResult($query);
	
	// Check that this session ID does not already exist.
	$query = SQL_Query
	(
		$mysql_database,
		"SELECT * FROM sessions WHERE sid = '$sid' LIMIT 1"
	);
	
	if (SQL_GetNumRows($query))
		Result_Error($ERROR_SESSION_EXISTS);
	
	SQL_FreeQueryResult($query);

	// ==================================================
	// Add the session to the database.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"INSERT INTO sessions ".
		"VALUES ".
		"(".
			"0, ".
			"'$gid', ".
			"$time, ".
			"$expiry, ".
			"'$owner', ".
			"'$sid', ".
			"'$pass', ".
			"'$ip', ".
			"$state, ".
			"'$title', ".
			"$tslots, ".
			"$uslots, ".
			"'$players', ".
			"'$info'".
		")"
	);
	
	$result = Result_Init();
	Result_AddValue($result, "pass", $pass);
	Result_Print($result);

?>