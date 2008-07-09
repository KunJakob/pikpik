<?php

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$gid		= "PikPik-1";
	$limit		= 25;
	
	// ==================================================
	// Mark all expired sessions as closed.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"UPDATE sessions SET state = $STATUS_TIMEOUT WHERE state = $STATUS_ACTIVE AND expiry < $current_time"
	);
	
	// ==================================================
	// Find all relevant sessions.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"SELECT sid, ip, title, tslots, uslots ". 
		"FROM sessions ".
		"WHERE gid = '$gid' ".
		"AND state = $STATUS_ACTIVE ".
		"AND expiry > $current_time ".
		"LIMIT $limit"
	);
	
	if (!$query)
	{
		echo "Query failed...";
		die;
	}
	
	$num_results = SQL_GetNumRows($query);
	
	if (!$num_results)
	{
		echo "No results to display...";
		die;
	}
	
	echo "<font face=\"Verdana\" size=\"1\">";
	
	while ($row = SQL_FetchRow($query))
	{
		echo "<b>Session ID:</b> ". $row['sid'] .", ";
		echo "<b>IP:</b> ". $row['ip'] .", ";
		echo "<b>Title:</b> ". $row['title'] .", ";
		echo "<b>Total Slots:</b> ". $row['tslots'] .", ";
		echo "<b>Used Slots:</b> ". $row['uslots'];
	
		echo "<br>";
	}
	
	echo "</font>";
	
	SQL_FreeQueryResult($query);

?>