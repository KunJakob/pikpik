<?php

	// ==================================================
	// Initialise variables for the operation.
	// ==================================================
	$gid		= $stream_values["gid"];
	$limit		= min($stream_values["limit"], $MAX_RESULTS);
	
	// ==================================================
	// Mark all expired sessions as closed.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"UPDATE matchmaking SET state = $STATUS_TIMEOUT WHERE state = $STATUS_ACTIVE AND expiry < $current_time"
	);
	
	// ==================================================
	// Find all relevant sessions.
	// ==================================================
	$query = SQL_Query
	(
		$mysql_database,
		"SELECT sid, ip, title, tslots, uslots, info ". 
		"FROM matchmaking ".
		"WHERE gid = '$gid' ".
		"AND state = $STATUS_ACTIVE ".
		"AND expiry > $current_time ".
		"LIMIT $limit"
	);
	
	if (!$query)
		Result_Error($ERROR_QUERY_FAILED);
	
	$num_results = SQL_GetNumRows($query);
	
	if (!$num_results)
		Result_Error($ERROR_NO_RESULTS);
	
	Result_Success();
	Result_Value("results", $num_results);
	
	$index = 0;
	
	while ($row = SQL_FetchRow($query))
	{
		Result_Value("sid:". $index, $row['sid']);
		Result_Value("ip:". $index, $row['ip']);
		Result_Value("title:". $index, $row['title']);
		Result_Value("tslots:". $index, $row['tslots']);
		Result_Value("uslots:". $index, $row['uslots']);
		Result_Value("info:". $index, $row['info']);
	
		$index++;
	}
	
	SQL_FreeQueryResult($query);
	
?>