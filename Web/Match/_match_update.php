<?php

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$expiry		= $current_time + $SESSION_TIMEOUT;
	$sid		= $stream_values["sid"];
	$pass		= $stream_values["pass"];
	$state		= $stream_values["state"];
	$uslots		= $stream_values["uslots"];
	$info		= $stream_values["info"];
	
	// ==================================================
	// Check the credentials of the session.
	// ==================================================
	
	// Verify the session ID.
	
	// Verify the password.
	
	// ==================================================
	// Update the session details including expiry.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"UPDATE sessions ".
		"SET ".
			"expiry = $expiry, ".
			"state = $state, ".
			"uslots = $uslots, ".
			"info = '$info' ".
		"WHERE sid = '$sid' AND pass = '$pass'"
	);
	
	if (!$query)
		Result_Error($ERROR_QUERY_FAILED);
		
	Result_Success();

?>