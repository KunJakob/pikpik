<?php

	// Error codes.
	$ERROR_SUCCESS				= 0;
	$ERROR_QUERY_FAILED			= 1;
	$ERROR_OWNER_EXISTS			= 2;
	$ERROR_SESSION_EXISTS		= 3;
	$ERROR_INVALID_SESSION		= 4;
	$ERROR_INVALID_PASSWORD		= 5;
	$ERROR_NO_RESULTS			= 6;

	// The session status.
	$STATUS_UNKNOWN 			= 0;
	$STATUS_CREATING			= 1;
	$STATUS_ACTIVE				= 2;
	$STATUS_STARTED				= 3;
	$STATUS_CLOSING				= 4;
	$STATUS_CLOSED				= 5;
	$STATUS_TIMEOUT 			= 6;
	
	// Matchmaking.
	$SESSION_TIMEOUT			= 30;
	$MAX_RESULTS				= 15;

?>