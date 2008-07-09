<?php

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$gid		= "PP001";
	$limit		= 25;
	$state		= $STATUS_ACTIVE;
	
	// ==================================================
	// Find all relevant sessions.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"SELECT sid, ip, title, tslots, uslots ". 
		"FROM sessions ".
		"WHERE gid = '$gid' ".
		"AND state = $state ".
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

?>