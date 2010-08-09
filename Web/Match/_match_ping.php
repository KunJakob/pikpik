<?php

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$expiry		= $current_time + $SESSION_TIMEOUT;
	$sid		= $stream_values["sid"];
	$pass		= $stream_values["pass"];
	
	// ==================================================
	// Check the credentials of the session.
	// ==================================================
	
	// Verify the session ID.
	
	// Verify the password.
	
	// ==================================================
	// Update the expiry for the session.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"UPDATE matchmaking SET expiry = $expiry WHERE sid = '$sid' AND pass = '$pass' AND state = $STATUS_ACTIVE"
	);
	
	if (!$query)
		Result_Error($ERROR_QUERY_FAILED);
		
	Result_Success();

?>