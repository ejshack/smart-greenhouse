<?php
    $message = '';

    // appends $msg to $message which is then displayed at the bottom
    function addMsg($msg)
    {
        global $message;
        $message .= $msg . '</br>';
    }

    $settingsRoot = '/plant/settings/';
    if (!file_exists($settingsRoot) && !mkdir($settingsRoot, 0775))
    {
        die("Error: Unable to create /plant/settings/ directory.");
    }

    // set the numPlants.txt file if the post variable is set
    $numPlantsFile = $settingsRoot . 'num_plants.txt';
    $numPlants = $_POST['numPlants'];
    $numPlants = intval($numPlants);
    if (isset($numPlants) && !empty($numPlants) && is_int($numPlants))
    {
        if(!file_put_contents($numPlantsFile, ''.$numPlants))
        {
            addMsg('FAILED TO WRITE ' . $numPlants . ' TO ' . $waterContentFile);
        }
        else
        {
            addMsg('Set number of plants to ' . $numPlants);
        }
    }
    // the variable was not set, we need to read the numPlants.txt file to get numPlants
    else
    {
        addMsg("No value set for number of plants or number of plants was not a number, not writing number of plants.");
        $numPlants = file_get_contents($numPlantsFile);
        addMsg('Read number of plants from file.');
        $numPlants = intval($numPlants);
    }

    $alertSubscribersFile = $settingsRoot . 'alert_subscribers.txt';
    $alertSubscribers = $_POST['alertSubscribers'];
    if (isset($alertSubscribers))
    {
        if(!file_put_contents($alertSubscribersFile, ''.$alertSubscribers))
        {
            addMsg('FAILED TO WRITE ' . $alertSubscribers . ' TO ' . $alertSubscribersFile);
        }
        else
        {
            addMsg('Set subscribers to ' . $alertSubscribers);
        }
    }
    // the variable was not set, we need to read the alert_subscribers.txt file to get subscribers
    else
    {
        addMsg("No value set for subscribers, not writing to subscribers.");
        $alertSubscribers = file_get_contents($alertSubscribersFile);
        if (!$alertSubscribers)
        {
            addMsg('Error with reading alertSubscribers file');
        }
		else
		{
			addMsg('Read alertSubscribers from file.');
		}

    }
    // counts the lines in $alertSubscribers
    $numSubscribers = count(explode("\n",$alertSubscribers));

    // sets max volume
    $maxVolumeFile = $settingsRoot . 'max_volume.txt';
    $maxVolume = $_POST['maxVolume'];
    if (isset($maxVolume) && !empty($maxVolume))
    {
        if(!file_put_contents($maxVolumeFile, ''.$maxVolume))
        {
            addMsg('FAILED TO WRITE ' . $maxVolume . ' TO ' . $maxVolumeFile);
        }
        else
        {
            addMsg('Set maxVolume to ' . $maxVolume);
        }
    }
    // the variable was not set, we need to read the max_volume.txt file
    else
    {
        addMsg("No value set for maxVolume, not writing to maxVolume.");
        $maxVolume = file_get_contents($maxVolumeFile);
        addMsg('Read maxVolume from file.');
    }

    // sets delayTime
    $delayTimeFile = $settingsRoot . 'delay_time_seconds.txt';
    $delayTime = $_POST['delayTime'];
    if (isset($delayTime) && !empty($delayTime))
    {
        if(!file_put_contents($delayTimeFile, ''.round($delayTime)))
        {
            addMsg('FAILED TO WRITE ' . $delayTime . ' TO ' . $delayTimeFile);
        }
        else
        {
            addMsg('Set delayTime to ' . $delayTime);
        }
    }
    // the variable was not set, we need to read the max_volume.txt file
    else
    {
        addMsg("No value set for delayTime, not writing to delayTimeFile.");
        $delayTime = file_get_contents($delayTimeFile);
        addMsg('Read delayTime from file.');
    }

    // sets watering time
    $wateringTimeSecondsFile = $settingsRoot . 'watering_time_seconds.txt';
    $wateringTimeSeconds = $_POST['wateringTimeSeconds'];
    if (isset($wateringTimeSeconds) && !empty($wateringTimeSeconds))
    {
        if(!file_put_contents($wateringTimeSecondsFile, ''.round($wateringTimeSeconds,2)))
        {
            addMsg('FAILED TO WRITE ' . $wateringTimeSeconds . ' TO ' . $wateringTimeSecondsFile);
        }
        else
        {
            addMsg('Set wateringTimeSeconds to ' . $wateringTimeSeconds);
        }
    }
    // the variable was not set, we need to read the max_volume.txt file
    else
    {
        addMsg("No value set for wateringTimeSeconds, not writing to wateringTimeSecondsFile.");
        $wateringTimeSeconds = file_get_contents($wateringTimeSecondsFile);
        addMsg('Read wateringTimeSeconds from file.');
    }

	//sets current volume to max volume
	$currentVolumeFile = $settingsRoot . 'current_volume.txt';
	if($_POST['resetCurrentVolume'])
	{
		if(!file_put_contents($currentVolumeFile, ''.$maxVolume))
        {
            addMsg('FAILED TO WRITE ' . $maxVolume . ' TO ' . $currentVolumeFile);
        }
        else
        {
            addMsg('Set current volume to to ' . $maxVolume);
        }
	}

    // sets alertVolume
    $alertVolumeFile = $settingsRoot . 'alert_volume.txt';
    $alertVolume = $_POST['alertVolume'];
    if (isset($alertVolume) && !empty($alertVolume))
    {
        if(!file_put_contents($alertVolumeFile, ''.$alertVolume))
        {
            addMsg('FAILED TO WRITE ' . $alertVolume . ' TO ' . $alertVolumeFile);
        }
        else
        {
            addMsg('Set alertVolume to ' . $alertVolume);
        }
    }
    // the variable was not set, we need to read the max_volume.txt file
    else
    {
        addMsg("No value set for alertVolume, not writing to alertVolume.");
        $alertVolume = file_get_contents($alertVolumeFile);
        addMsg('Read alertVolume from file.');
    }

    // sets water content thresholds
    $waterContentFile = $settingsRoot . 'water_content.txt';
    if (is_int($numPlants))
    {
        $waterContentStr = '';
        // see if the waterContent array was set and try to write to file
        for($i = 0; $i < $numPlants; $i++)
        {
            // get the water content for this plant
            $content = $_POST['waterContent' . $i];


            // if this plants water content is not set or is not a number
            // there is an error, do not write to file
            if (!is_numeric($content))
            {
                $waterContentStr = null;
                addMsg("Water Content not set or not a number for plant ".$i.". Not writing water content at all.");
                break;
            }

            // cast to an float
            $content = floatval($content);

            // concatenate waterContent String to write to file
            // appending a ',' on all but the last entries
            if ($i < $numPlants - 1)
            {
                $waterContentStr .= $content . ',';
            }
            else
            {
                $waterContentStr .= $content;
            }
        }

        // write the string to the end of the water content file
        if (!empty($waterContentStr))
        {
            // append the new watercontent on the last line of the file
            if(!file_put_contents($waterContentFile, "\n" . $waterContentStr, FILE_APPEND))
            {
                addMsg('FAILED TO WRITE ' . $waterContentStr . ' TO ' . $waterContentFile);
            }
            else
            {
                addMsg('Wrote ' . $waterContentStr . ' to ' . $waterContentFile . '.');
            }

            // create an array of water contents from the string
            $waterContent = explode(',', $waterContentStr);
        }
        else
        {
            // we need to read from the water content file to get the water contents
            // read water contents file
            $waterContents = file_get_contents($waterContentFile);

            // separate into array of lines
            $waterContents = explode("\n", $waterContents);

            // get the latest good line in the file
            for ($i = count($waterContents) - 1; $i >= 0; $i--)
            {
                // get the last line
                $waterContent = $waterContents[$i];

                // break the line up by ',' delimiter character
                $waterContent = explode(',', $waterContent);

                // check first that there is an entry for each plant,
                // else there is an error and we need to try the next line
                if (count($waterContent) != $numPlants)
                {
                    continue;
                }

                // check that all entries are valid in this line
                $allEntriesAreNumber = true;
                for ($j = 0; $j < $numPlants; $j++)
                {
                    // if the entry is not a number it is invalid
                    $waterContent[$j] = floatval($waterContent[$j]);
                    if (!is_float($waterContent[$j]))
                    {
                        $allEntriesAreNumber = false;
                        break;
                    }
                }
                if (!$allEntriesAreNumber)
                {
                    continue;
                }

                // at this point the line is valid so break
                addMsg('Using water content ' . $waterContents[$i] . ' from line ' . $i . ' of ' . $waterContentFile);
                break;
            }
        }
    }
    else
    {
        addMsg("Number of plants is not a number!");
    }

    // if there is no waterConent just intialize it to an empty array
    if (!isset($waterContent) || empty($waterContent))
    {
        addMsg('No water content set.');
        $waterContent = [];
    }
?>

<html>
    <head>
        <title>Change Settings</title>
        <!-- Style sheets -->
        <link rel="stylesheet" type="text/css" href="css/bootstrap.min.css">
        <link rel="stylesheet" type="text/css" href="css/style.css">
        <link rel="stylesheet" type="text/css" href="css/bootstrap-theme.min.css">
        <style>
            .content
            {
                width: 90%;
                margin-left: 5%;
            }
            .plantchart
            {
                float:left;
                padding-left: 10px;
            }
            .container
            {
                display: inline-block;
            }
        </style>
    </head>

    <div class="container">
        <body>
            <h1>Change Settings</h1>
            <a href="index.php">Home</a></br></br>
            <h2>Plant Settings</h2>
            <form action='changeSettings.php' method='post'>
                <label>Number of Plants: </label>
                <input type='text' name='numPlants' value="<?php echo $numPlants;?>"/></br>

                <?php
                $contentCount = count($waterContent);
                for($i=0;$i<$numPlants;$i++)
                {
                    $content = $contentCount >= $i ? $waterContent[$i] : '';
                    echo '<label>Threshold for plant ' . ($i+1) . ' </label> ';
                    echo '<input type="text" name="waterContent'.$i.'" value="' . $content . '"/></br>';
                }
                ?>

                <h2>Pump Settings</h2>
                <label>Seconds to delay after pumping: </label>
                <input type='text' name='delayTime' value="<?php echo $delayTime;?>"/></br>
                <label>Seconds to pump: </label>
                <input type='text' id="secondsToPump" name='wateringTimeSeconds' value="<?php echo round($wateringTimeSeconds,2);?>"/></br>
                <label>Millilitres to pump: </label>
                <input type='text'  id="millilitersToPump" value="<?php echo round($wateringTimeSeconds * (55.004 / 60.0),2);?>"/></br>

                <h2>Email Alert Subscribers</h2>
                <textarea rows="<?php echo $numSubscribers;?>" cols="50" name="alertSubscribers"><?php echo $alertSubscribers;?></textarea>
                </br></br>

                <label>Fill-line volume (mL): </label>
                <input type='text' name='maxVolume' value="<?php echo $maxVolume;?>"/></br></br>

                <label>Alert at water volume (mL): </label>
                <input type='text' name='alertVolume' value="<?php echo $alertVolume;?>"/></br></br>

                <label>Reset current volume to fill line </label>
                <input type='checkbox' name='resetCurrentVolume'/></br></br>
                <input type='submit'/>
            </form>
            <div id="messageDiv"><?php echo $message;?></div>
        </body>
    </div>
</html>

<script>
    function roundToHundreth(num)
    {
        return Math.ceil(num* 100) / 100;
    }

    var secondsToPump = document.getElementById('secondsToPump');
    secondsToPump.onkeyup = function(){
        document.getElementById('millilitersToPump').value = roundToHundreth(secondsToPump.value * (55.004 / 60.0));
    }

    var mLToPump = document.getElementById('millilitersToPump');
    mLToPump.onkeyup = function(){
        document.getElementById('secondsToPump').value = roundToHundreth(mLToPump.value * (60.0 / 55.004));
    }
</script>
