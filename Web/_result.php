<?php

	function Result_Error($error_code)
	{
		echo "result://error=$error_code";
		exit;
	}

	// Initialise a success result string.
	function Result_Init()
	{
		return "result://error=0";
	}

	// Add a key-value pair to the result text.
	function Result_AddValue(&$result, $key, $value)
	{
		$result .= "&$key=$value";
	}
	
	// Add a section separator.
	function Result_AddSeparator(&$result)
	{
		$result .= "//";
	}
	
	// Print the result to the screen.
	function Result_Print($result)
	{
		echo $result;
	}

?>