<?php

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$sid		= $stream_values["sid"];
	$pass		= $stream_values["pass"];
	$state		= $STATUS_CLOSED;
	
	// ==================================================
	// Check the credentials of the session.
	// ==================================================
	
	// Verify the session ID.
	
	// Verify the password.
	
	// ==================================================
	// Set the session state to closed.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"UPDATE sessions SET state = $state WHERE sid = '$sid' AND pass = '$pass'"
	);
	
	if (!$query)
		Result_Error($ERROR_QUERY_FAILED);
		
	Result_Success();
?>