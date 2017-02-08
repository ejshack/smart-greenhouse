<?php
	require_once("sqlConnection.php");
?>
<!doctype html>
<html>
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <title>Plant Monitor</title>

        <!-- Style sheets -->
        <link rel="stylesheet" type="text/css" href="css/bootstrap.min.css">
        <link rel="stylesheet" type="text/css" href="css/style.css">
        <link rel="stylesheet" type="text/css" href="css/bootstrap-theme.min.css">

        <!-- Javascript includes -->
        <script type="text/javascript" src="js/Chart.Core.js"></script>
        <script type="text/javascript" src="js/Chart.Bar.js"></script>
        <script type="text/javascript" src="js/Chart.Line.js"></script>
        <script type="text/javascript" src="js/jquery-latest.js"></script>
        <script type="text/javascript" src="js/jquery.tablesorter.min.js"></script>
        <script type="text/javascript" src="js/jquery-2.1.3.min.js"></script>
        <script type="text/javascript" src="js/bootstrap.min.js"></script>
        <script type="text/javascript" src="js/jquery.metadata.js"></script>
        <style>
            .content
            {
                width: 90%;
                margin-left: 5%;
            }
        </style>
    </head>
    <body>
        <div class="content">
            <h1>Plant <?php echo intval($_GET['id'])+1;?> Statistics</h1>
            <div>
                <a href="changeSettings.php">Click here to change plant settings</a></br>
                <a href="index.php">Home</a></br></br>
            </div>
        </div>
        <div id="content" class="wide">
            <div id="HTMLBlock" class="content">
                <?php
                    /*
                     * Query the database and get all necessary data for this plant id.
                     *
                     * Each plant will have readings of soil moisture, leaf thickness, and water usage at many different times.
                     * Because this page is to display information for this plant then we need to get all readings from this plant
                     * and to make a separate graph for each type of data we will separate the data into arrays and display each
                     * array in a separate graph.
                     */
                    $NUM_LABELS = 20;

                    $labels = [];
                    $moistureData = [];
                    $thicknessData = [];
                    $waterUsageData = [];
                    if (isset($_GET['id']) && $_GET['id'] >= 0)
                    {
                        $id = $db->real_escape_string($_GET['id']);
                        $result = $db->query("SELECT * FROM `PlantMonitor_Data` WHERE `PLANT_ID` = '" . $id . "' ORDER BY TIME");
                        $resultLen = $result->num_rows;

                        $i=0;
                        foreach($result as $key => $val)
                        {
                            /*
                             * Evenly space the labels over the result data with NUM_LABELS number of labels.
                             */
                            if($i%round($resultLen/$NUM_LABELS)==0)
                            {
                                $labels[] = $val['TIME'];
                            }
                            $moistureData[] = $val['MOISTURE_PERCENTAGE'];
                            $thicknessData[] = $val['LEAF_THICKNESS'];
                            $waterUsageData[] = $val['WATER_USED_MILLILITERS'];
                            $i++;
                        }
                    }
                ?>
                <script>
                    var moistureDataSet =
                    {
                        labels :
                        <?php echo json_encode($labels);?>,
                        datasets :
                        [
                            {
                                fillColor : "rgba(63,127,191,1)",
                                strokeColor : "rgba(220,220,220,1)",
                                pointColor : "rgba(220,220,220,0)",
                                pointStrokeColor : "#fff",
                                pointHighlightFill : "#fff",
                                pointHighlightStroke : "rgba(220,220,220,1)",
                                data : <?php echo json_encode($moistureData);?>
                            }
                        ]
                    };
                    var thicknessDataSet =
                    {
                        labels :
                        <?php echo json_encode($labels);?>,
                        datasets :
                        [
                            {
                                fillColor : "rgba(244,182,34,1)",
                                strokeColor : "rgba(220,220,220,1)",
                                pointColor : "rgba(220,220,220,0)",
                                pointStrokeColor : "#fff",
                                pointHighlightFill : "#fff",
                                pointHighlightStroke : "rgba(220,220,220,1)",
                                data : <?php echo json_encode($thicknessData);?>
                            }
                        ]
                    };
                    var waterUsageDataSet =
                    {
                        labels :
                        <?php echo json_encode($labels);?>,
                        datasets :
                        [
                            {
                                fillColor : "rgba(63,127,191,1)",
                                strokeColor : "rgba(220,220,220,1)",
                                pointColor : "rgba(220,220,220,0)",
                                pointStrokeColor : "#fff",
                                pointHighlightFill : "#fff",
                                pointHighlightStroke : "rgba(220,220,220,1)",
                                data : <?php echo json_encode($waterUsageData);?>
                            }
                        ]
                    };


                    window.onload = function(){
                        var canvasbar = document.getElementById("moistureCanvas").getContext("2d");
                        window.myBar = new Chart(canvasbar).Line(moistureDataSet, {
                            responsive : true
                        });
                        var canvasbar = document.getElementById("thicknessCanvas").getContext("2d");
                        window.myBar = new Chart(canvasbar).Line(thicknessDataSet, {
                            responsive : true
                        });
                        var canvasbar = document.getElementById("waterUsageCanvas").getContext("2d");
                        window.myBar = new Chart(canvasbar).Line(waterUsageDataSet, {
                            responsive : true
                        });
                    }
                </script>
                <div class="plantchart">
                    <h2>Moisture Content</h2>
                    <canvas id="moistureCanvas"></canvas>
                </div>
                <div class="plantchart">
                    <h2>Thickness Millimeters</h2>
                    <canvas id="thicknessCanvas"></canvas>
                </div>
                <div class="plantchart">
                    <h2>Water Used (mL)</h2>
                    <canvas id="waterUsageCanvas"></canvas>
                </div>
                <div id="copyright">
                    <div class="repository">
                        <div class="left">
                            <p id="copy">This site © 2015 Iowa State University. All Rights Reserved.</p>
                            <div class="clear"></div>
                        </div>
                        <div class="right">
                            <a target="_blank" href="http://dec1514.sd.ece.iastate.edu/">Senior Design Group DEC15-14</a>
                            <div class="clear"></div>
                        </div>
                        <div class="clear"></div>
                    </div>
                </div>
            </div>
        </div>
    </body>
</html>
