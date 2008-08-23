<?php

	function Result_Error($error_code)
	{
		echo "result://error=$error_code";
		exit;
	}

	// Initialise a success result string.
	function Result_Success()
	{
		echo "result://error=0";
	}

	// Add a key-value pair to the result text.
	function Result_Value($key, $value)
	{
		if (strlen($key) && strlen($value))
			echo "&$key=$value";
	}

?>