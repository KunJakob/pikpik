<?php

	// Verify that a session ID and password matches the database.
	function Verify_SessionPassword($sid, $pass)
	{
		$query = SQL_Query
		(
			"SELECT * FROM sessions WHERE sid = '$sid' AND pass = '$pass' LIMIT 1"
		);
		
		if ($query && SQL_GetNumRows($query))
		{
			SQL_FreeQueryResult($query);
			return true;
		}
		
		return false;
	}

?>