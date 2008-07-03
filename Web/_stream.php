<?php

	// Read an incoming POST steam directly into a buffer of specified size.
	function Stream_ReadBuffer()
	{
		$stream_file = fopen("php://input", "rb");
		
		while (!feof($stream_file))
			$stream_data .= fread($stream_file, 512);
			
		fclose($stream_file);
		
		$stream_header = "match://";
		
		if (strpos($stream_data, $stream_header) == 0)
		{
			return substr($stream_data, strlen($stream_header));
		}
		
		return false;
	}

	// Process the stream data and break into key-value pairs.
	function Stream_GetKeyPairs($stream_data)
	{
		return explode('&', $stream_data);
	}

	// Get the key from a key-value pair.
	function Stream_GetKey($key_pair)
	{
		$components = explode('=', $key_pair);
		
		if ($components[0])
		{
			return $components[0];
		}
		
		return false;
	}

	// Get the value from a key-value pair.
	function Stream_GetValue($key_pair)
	{
		$components = explode('=', $key_pair);
		
		if ($components[1])
		{
			return $components[1];
		}
		
		return false;
	}
	
	// Generate an associative array of all the key-pairs from the incoming stream.
	function Stream_GenerateAssocArray()
	{
		$stream_buffer = Stream_ReadBuffer();
		
		if ($stream_buffer)
		{
			$final_array = false;
			$key_pairs = Stream_GetKeyPairs($stream_buffer);
			
			foreach ($key_pairs as $kp)
			{
				$key = Stream_GetKey($kp);
				$value = Stream_GetValue($kp);
				
				if ($key && $value)
				{
					$final_array[$key] = $value;
				}
			}
			
			return $final_array;
		}
	
		return false;
	}

?>