<?php
	$db = new mysqli("localhost", "plants", "8SEh2R7LsFQAJnuM", "plants");
    if($db->connection_error)
    {
        die("connection failed: " . $db->connection_error);
    }
?>
