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
	$gid		= $_POST["gid"];
	$time		= $current_time;
	$expiry		= $current_time + $SESSION_TIMEOUT;
	$owner		= $_SERVER["REMOTE_ADDR"];
	$sid		= $_POST["sid"];
	$pass		= GeneratePassword();
	$ip			= $_SERVER["REMOTE_ADDR"];
	$port		= 0;
	$state		= $STATUS_ACTIVE;
	$title		= $_POST["title"];
	$tslots		= $_POST["tslots"];
	$uslots		= $_POST["uslots"];
	$players	= "";
	$info		= $_POST["info"];
	
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
		"SELECT * FROM sessions WHERE owner = '$owner' AND expiry > $time AND state = $STATUS_ACTIVE LIMIT 1"
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
			"$port, ".
			"$state, ".
			"'$title', ".
			"$tslots, ".
			"$uslots, ".
			"'$players', ".
			"'$info'".
		")"
	);
	
	if (!$query)
		Result_Error($ERROR_QUERY_FAILED);
	
	Result_Success();
	Result_Value("pass", $pass);

?>