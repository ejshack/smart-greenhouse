/* js functionality for sg-helper plugin */
jQuery(document).ready(function($) {

	// // active chamber button to display chamber table
	// $('#sg-chamber-button').click(function() {
	// 	sgCharts.setActive(null);
	// 	sgCharts.setIsActive(true);
	// 	// sgCharts.chamberTable.clearChart();
	// 	$('#sg-chamberinfo').hide();
	// 	$('#sg-notactive').hide();
	// 	$('#sg-dashboard-div').hide();
	// 	chamberTableInit();
	// });
	//
	// // chamber history button to display previously deactivated chambers
	// $('#sg-history-button').click(function() {
	// 	sgCharts.setActive(null);
	// 	sgCharts.setIsActive(false);
	// 	// sgCharts.chamberTable.clearChart();
	// 	$('#sg-chamberinfo').hide();
	// 	$('#sg-notactive').hide();
	// 	$('#sg-dashboard-div').hide();
	// 	chamberTableInit();
	// });
	$('#reload-chambers').click(function() {
		$('#sg-dashboard-div').hide();
		$('#sg-chamberinfo').hide();
		$('#sg-notactive').hide();
		var ctView = new google.visualization.DataView(sgCharts.dtChamberTable);
		ctView.setColumns([0,1,2,3,4,5]);
		sgCharts.chamberTable.draw(ctView, sgCharts.getCTOptions());
	});

	// update chamber details textarea in chamberinfo div
	$('#sg-update-details').click(function() {
		var details = $('#sg-update-details-text').val();
		var data_ud = {
			'action' : 'update_details',
			'security' : ajax_object.security,
			'number' : sgCharts.getActive(),
			'details' : details
		}
		jQuery.post(ajax_object.ajaxurl, data_ud, function(response) {
			// update chamber table
			sgCharts.dtChamberTable.setCell((sgCharts.getActive() - 1), 6, details);
			// sgCharts.chamberTable.draw(sgCharts.dtChamberTable, sgCharts.getCTOptions());
			// var ctView = new google.visualization.DataView(sgCharts.dtChamberTable);
			// ctView.setColumns([0,1,2,3,4,5]);
			// sgCharts.chamberTable.draw(ctView, sgCharts.getCTOptions());
			$('html, body').animate({ scrollTop: 0 }, 'fast');
		});
	});

	// add chamber form for activating new chamber
	$('#add-chamber').click(function() {
		// forces title to be required, alert and return
		if( !$('#add-title').val().trim() ) {
			alert('Title is required');
			return;
		}
		// collect and prepare input data for post call
		var data_ac = {
			'action': 'add_chamber',
			'security': ajax_object.security,
			'number': sgCharts.getActive(),
			'title': $('#add-title').val(),
			'type': $('input[name=sg-add-type]:checked').val(),
			'interval_length' : $('#add-interval').val(),
			'interval_type' : $('#add-interval-type option:selected').val(),
			'details': $('#add-details').val()
		}
		// send request
		jQuery.post(ajax_object.ajaxurl, data_ac, function(response) {
			// hide add chamber form
			$('#sg-notactive').hide();
			$('#add-title').val('');
			// update chamber table
			var row = sgCharts.getActive() - 1;
			sgCharts.dtChamberTable.setCell(row, 1, true);
			sgCharts.dtChamberTable.setCell(row, 2,
				(data_ac['type'] === 'C' ? 'Cooling' : 'Heating') );
			sgCharts.dtChamberTable.setCell(row, 3, data_ac['title']);
			sgCharts.dtChamberTable.setCell(row, 4, data_ac['interval_length']);
			sgCharts.dtChamberTable.setCell(row, 5, data_ac['interval_type']);
			sgCharts.dtChamberTable.setCell(row, 6, data_ac['details']);
			// sgCharts.chamberTable.draw(sgCharts.dtChamberTable, sgCharts.getCTOptions());
			var ctView = new google.visualization.DataView(sgCharts.dtChamberTable);
			ctView.setColumns([0,1,2,3,4,5]);
			sgCharts.chamberTable.draw(ctView, sgCharts.getCTOptions());
			// show new chamber info
			$('#chamber-number').text('Chamber ' + data_ac['number']);
			$('#chamber-title').text(data_ac['title']);
			if(data_ac['type'] === 'C') {
				$('#chamber-type').text('Cooling Chamber');
			} else {
				$('#chamber-type').text('Heating Chamber');
			}
			$('#sg-update-details-text').val(data_ac['details']);
			$('#sg-chamberinfo').show();
		});
	}); // end add chamber form click handler

	// cancel add chamber button
	$('#cancel-add-chamber').click(function() {
		$('#sg-notactive').hide();
		$('#add-title').val('');
		$('#add-details').val('Details and notes about the chamber.');
	}); // end cancel add chamber

	// deactivate chamber button
	$('#deactivate-chamber').click(function() {
		var data_dc = {
			'action' : 'deactivate_chamber',
			'security' : ajax_object.security,
			'number' : sgCharts.getActive()
		}
		jQuery.post(ajax_object.ajaxurl, data_dc, function(response) {
			$('#sg-chamberinfo').hide();
			$('#sg-dashboard-div').hide();
			// update chamber table
			var row = sgCharts.getActive() - 1;
			sgCharts.dtChamberTable.setCell(row, 1, false);
			sgCharts.dtChamberTable.setCell(row, 3, null);
			sgCharts.dtChamberTable.setCell(row, 4, 0);
			sgCharts.dtChamberTable.setCell(row, 5, null);
			sgCharts.dtChamberTable.setCell(row, 6, null);
			// sgCharts.chamberTable.draw(sgCharts.dtChamberTable, sgCharts.getCTOptions());
			var ctView = new google.visualization.DataView(sgCharts.dtChamberTable);
			ctView.setColumns([0,1,2,3,4,5]);
			sgCharts.chamberTable.draw(ctView, sgCharts.getCTOptions());
			// update chamber info div
			$('#chamber-number').text('');
			$('#chamber-title').text('');
			$('#chamber-type').text('');
			$('#chamber-details').text('');
			sgCharts.setActive(null);
		});
	}); // end deactivate chamber


	/* Initial Chamber Table Load */
	google.charts.setOnLoadCallback(chamberTableInit);

	/*
	 * Load chamber data and create chamber table.
	 */
	function chamberTableInit() {
		sgCharts.dtChamberTable = new google.visualization.DataTable();
		// only add chamber number and active column to active chamber table
		// if(sgCharts.getIsActive()) {
			sgCharts.dtChamberTable.addColumn('number', '#');
			sgCharts.dtChamberTable.addColumn('boolean', 'Active');
		// }
		sgCharts.dtChamberTable.addColumn('string', 'Type');
		sgCharts.dtChamberTable.addColumn('string', 'Title');
		sgCharts.dtChamberTable.addColumn('number', 'Interval Length');
		sgCharts.dtChamberTable.addColumn('string', 'Interval Type');
		sgCharts.dtChamberTable.addColumn('string', 'Details');

		var data_ct = {
			'action': 'chamber_table_init',
			'security': ajax_object.security//,
			// 'isActive' : sgCharts.getIsActive()
		}
		jQuery.post(ajax_object.ajaxurl, data_ct, function(response) {
			// populate chamber table
			var chambers = JSON.parse(response.stripSlashes());
			for(var i = 0; i < chambers.length; ++i) {
				// if(sgCharts.getIsActive()) {
					// active chamber table
					sgCharts.dtChamberTable.addRow([
											 parseInt(chambers[i]['number']),
											 (chambers[i]['active'] == '1' ? true : false),
											 (chambers[i]['type'] === 'C' ? 'Cooling' : 'Heating'),
											 chambers[i]['title'],
											 parseInt(chambers[i]['interval_length']),
											 chambers[i]['interval_type'],
											 chambers[i]['details']
										 ]);
				// } else {
				// 	// chamber history table
				// 	sgCharts.dtChamberTable.addRow([
				// 							 (chambers[i]['type'] === 'C' ? 'Cooling' : 'Heating'),
				// 							 chambers[i]['title'],
				// 							 parseInt(chambers[i]['interval_length']),
				// 							 chambers[i]['interval_type'],
				// 							 chambers[i]['details']
				// 						 ]);
				// }

			}

			sgCharts.chamberTable = new google.visualization.Table(document.getElementById('chamber-table'));
			sgCharts.dashboard = new google.visualization.Dashboard(document.getElementById('sg-dashboard-div'));

			// create view to display table without details column
			var ctView = new google.visualization.DataView(sgCharts.dtChamberTable);
			// if(sgCharts.getIsActive()) {
				ctView.setColumns([0,1,2,3,4,5]);
			// } else {
			// 	ctView.setColumns([0,1,2,3]);
			// }

			sgCharts.chamberTable.draw(ctView, sgCharts.getCTOptions());
			// sgCharts.chamberTable.draw(sgCharts.dtChamberTable, sgCharts.getCTOptions());

			setCTSelectHandlers();
		});
	} // end initial chamber tables load

	/* Set listeners for chamber tables select event to get selected
	 * chamber's data, create graphs and draw the dashboard.
	 */
	function setCTSelectHandlers() {
		google.visualization.events.addListener(sgCharts.chamberTable, 'select', function() {
			// get selected row
			var selectedItem = sgCharts.chamberTable.getSelection()[0];
			if (selectedItem) {
				// get chamber number from row and set active chamber
				// if(!sgCharts.getIsActive()) {
					sgCharts.setActive(sgCharts.dtChamberTable.getValue(selectedItem.row, 0));
				// } else {
				// 	sgCharts.setActive(selectedItem.row + 1);
				// }
				// show inactive form and return if active chamber table chamber is inactive
				// if(sgCharts.getIsActive() && (sgCharts.dtChamberTable.getValue(selectedItem.row, 1) === false)) {
				if(sgCharts.dtChamberTable.getValue(selectedItem.row, 1) === false) {
					$('#sg-dashboard-div').hide();
					$('#sg-chamberinfo').hide();
					$('#add-number').text(sgCharts.getActive());
					$('#add-title').val('');
					$('#sg-notactive').show();
					return;
				}
				// hide inactive form if shown
				$('#sg-notactive').hide();
				// show general chamber info
				// if(sgCharts.getIsActive()){
					$('#chamber-number').text('Chamber ' + sgCharts.getActive());
					$('#chamber-title').text(sgCharts.dtChamberTable.getValue(selectedItem.row, 3));
					if(sgCharts.dtChamberTable.getValue(selectedItem.row, 2) === 'Cooling') {
						$('#chamber-type').text('Cooling Chamber');
					} else {
						$('#chamber-type').text('Heating Chamber');
					}
					$('#sg-update-details-text').val(sgCharts.dtChamberTable.getValue(selectedItem.row, 6));
					$('#deactivate-chamber').show();
				// } else {
				// 	$('#chamber-number').hide();
				// 	$('#deactivate-chamber').hide();
				// 	$('#sg-update-details').hide();
				// 	$('#chamber-title').text(sgCharts.dtChamberTable.getValue(selectedItem.row, 1));
				// 	if(sgCharts.dtChamberTable.getValue(selectedItem.row, 0) === 'Cooling') {
				// 		$('#chamber-type').text('Cooling Chamber');
				// 	} else {
				// 		$('#chamber-type').text('Heating Chamber');
				// 	}
				// 	$('#sg-update-details-text').val(sgCharts.dtChamberTable.getValue(selectedItem.row, 4));
				// }
				$('#sg-chamberinfo').show();
				// get data for selected table and populate tables in dashbaord
				setupDashboard();
			}
		});
	}  // end setCTSelectHandlers


  /* NEED TO LOOKUP INTERVAL FOR CHAMBER AND USE THAT HERE, PUT INSIDE setupDashboard */
	/* NEED TO ADD SCHEDULES TO CRON JOBS ON PHP AND ADD/REMOVE SCHEDULE ON ADD/REMOVE CHAMBER */
	// load dashboard on time interval
	setInterval(function() {
		if(sgCharts.getActive()) {
			updateDashboard();
		}
	}, 5000);


	/* Gets active chamber sensor readings and updates the dashbaord data */
	function updateDashboard() {
		var data = {
			'action' : 'sg_load_dashboard',
			'security' : ajax_object.security,
			'number' : sgCharts.getActive()//,
			// 'isActive' : sgCharts.getIsActive()
		}
		jQuery.post(ajax_object.ajaxurl, data, function(response) {
			// parse data
			var sensor_records = JSON.parse(response.stripSlashes());
			// return if chamber has no records
			if(sensor_records.length === 0) {
				$('#sg-dashboard-div').hide();
				return;
			}
			// only add new sensor readings, don't re-add data
			var numberOfRows = sgCharts.dtDashboard.getNumberOfRows() - 1;
			if(numberOfRows < sensor_records.length) {
				// add the new rows
				for(var i = numberOfRows; i < sensor_records.length; ++i) {
					sgCharts.dtDashboard.addRow([ new Date(sensor_records[i]['datetime']),
											 parseFloat(sensor_records[i]['o2_ppm']),
											 parseFloat(sensor_records[i]['co2_ppm']),
											 parseFloat(sensor_records[i]['temp_degC']),
											 parseFloat(sensor_records[i]['humidity'])
										 ]);
				}
				// save control (chart range filter) states
				var o2State = sgCharts.o2Control.getState();
				var co2State = sgCharts.co2Control.getState();
				var tempState = sgCharts.tempControl.getState();
				var humState = sgCharts.humControl.getState();

				sgCharts.dashboard.draw(sgCharts.dtDashboard);

				// keep previous start state, update end state to last data point
				var endRange = new Date(sensor_records[sensor_records.length-1]['datetime']);
				sgCharts.o2Control.setState({ 'range' : {
																					'start' : o2State.range.start,
																				   'end' : endRange
																				 }
																			 });
				sgCharts.co2Control.setState({ 'range' : {
																					'start' : co2State.range.start,
																				   'end' : endRange
																				 }
																			 });
				sgCharts.tempControl.setState({ 'range' : {
																					'start' : tempState.range.start,
																				   'end' : endRange
																				 }
																			 });
				sgCharts.humControl.setState({ 'range' : {
																					'start' : humState.range.start,
																				   'end' : endRange
																				 }
																			 });
			}
		});
	}

	/* Setup the dashboard using the chamber's sensor data (sensor_records) */
	function setupDashboard() { //sensor_records) {
		var data_ldb = {
			'action' : 'sg_load_dashboard',
			'security' : ajax_object.security,
			'number' : sgCharts.getActive()//,
			// 'isActive' : sgCharts.getIsActive()
		}
		jQuery.post(ajax_object.ajaxurl, data_ldb, function(response) {
			// parse data
			var sensor_records = JSON.parse(response.stripSlashes());
			// return if chamber has no records
			if(sensor_records.length === 0) {
				$('#sg-dashboard-div').hide();
				// alert('This chamber is active, but it has no sensor records yet. Please allow more time.')
				return;
			}
			// setupDashboard(sensor_records);
			// set datatable columns with general chamber info
			sgCharts.dtDashboard = new google.visualization.DataTable();
			sgCharts.dtDashboard.addColumn('datetime','Date and Time');
			sgCharts.dtDashboard.addColumn('number','O2 (ppm)');
			sgCharts.dtDashboard.addColumn('number','CO2 (ppm)');
			sgCharts.dtDashboard.addColumn('number','Temperature (C)');
			sgCharts.dtDashboard.addColumn('number','Humidity (%)');
			// fill in datatable rows with selected chambers sensor records
			for(var i = 0; i < sensor_records.length; ++i) {
				sgCharts.dtDashboard.addRow([ new Date(sensor_records[i]['datetime']),
										 parseFloat(sensor_records[i]['o2_ppm']),
										 parseFloat(sensor_records[i]['co2_ppm']),
										 parseFloat(sensor_records[i]['temp_degC']),
										 parseFloat(sensor_records[i]['humidity'])
									 ]);
			}

			// create charts and controls
			sgCharts.o2Chart = new google.visualization.ChartWrapper({
				'chartType' : 'LineChart',
				'containerId' : 'sg-o2-chart',
				'options' : {
					'legend' : 'none',
					// 'title' : 'O2 (ppm)',
					'width' : '100%',
					'height' : 400,
					'backgroundColor' : {
						'stroke' : '#ffffff',
						'strokeWidth' : 10,
						'fill' : '#000000'
					},
					'colors' : [ '#ff0000' ],
					'fontSize' : 16,
					'curveType' : 'function',
					'hAxis' : {
						'textStyle' : {
							'color' : '#ffffff'
						}
					},
					'vAxis' : {
						'title' : 'O2 (%)',
						'titleTextStyle' : {
							'color' : '#ffffff'
						},
						'textStyle' : {
							'color' : '#ffffff'
						},
						'format' : '#.##\'%\''
					}
				},
				'view' : {
					'columns' : [0,1]
				}
			});

			sgCharts.o2Control = new google.visualization.ControlWrapper({
				'controlType' : 'ChartRangeFilter',
				'containerId' : 'sg-o2-control',
				'options' : sgCharts.getControlOptions()
			});

			sgCharts.co2Chart = new google.visualization.ChartWrapper({
				'chartType' : 'LineChart',
				'containerId' : 'sg-co2-chart',
				'options' : {
					'legend' : 'none',
					// 'title' : 'CO2 (ppm)',
					'width' : '100%',
					'height' : 400,
					'backgroundColor' : {
						'stroke' : '#ffffff',
						'strokeWidth' : 10,
						'fill' : '#000000'
					},
					'colors' : [ '#ff0000' ],
					'fontSize' : 16,
					'curveType' : 'function',
					'hAxis' : {
						'textStyle' : {
							'color' : '#ffffff'
						}
					},
					'vAxis' : {
						'title' : 'CO2 (ppm)',
						'titleTextStyle' : {
							'color' : '#ffffff'
						},
						'textStyle' : {
							'color' : '#ffffff'
						}
					},
					'format' : '#.##'
				},
				'view' : {
					'columns' : [0,2]
				}
			});

			sgCharts.co2Control = new google.visualization.ControlWrapper({
				'controlType' : 'ChartRangeFilter',
				'containerId' : 'sg-co2-control',
				'options' : sgCharts.getControlOptions()
			});

			sgCharts.tempChart = new google.visualization.ChartWrapper({
				'chartType' : 'LineChart',
				'containerId' : 'sg-temp-chart',
				'options' : {
					'legend' : 'none',
					// 'title' : 'Temperature (C)',
					'width' : '100%',
					'height' : 400,
					'backgroundColor' : {
						'stroke' : '#ffffff',
						'strokeWidth' : 10,
						'fill' : '#000000'
					},
					'colors' : [ '#ff0000' ],
					'fontSize' : 16,
					'curveType' : 'function',
					'hAxis' : {
						'textStyle' : {
							'color' : '#ffffff'
						}
					},
					'vAxis' : {
						'title' : 'Temp (C)',
						'titleTextStyle' : {
							'color' : '#ffffff'
						},
						'textStyle' : {
							'color' : '#ffffff'
						},
						'format' : '#.##'
					}
				},
				'view' : {
					'columns' : [0,3]
				}
			});

			sgCharts.tempControl = new google.visualization.ControlWrapper({
				'controlType' : 'ChartRangeFilter',
				'containerId' : 'sg-temp-control',
				'options' : sgCharts.getControlOptions()
			});

			sgCharts.humChart = new google.visualization.ChartWrapper({
				'chartType' : 'LineChart',
				'containerId' : 'sg-humidity-chart',
				'options' : {
					'legend' : 'none',
					// 'title' : 'Humidity (%)',
					'width' : '100%',
					'height' : 400,
					'backgroundColor' : {
						'stroke' : '#ffffff',
						'strokeWidth' : 10,
						'fill' : '#000000'
					},
					'colors' : [ '#ff0000' ],
					'fontSize' : 16,
					'curveType' : 'function',
					'hAxis' : {
						'textStyle' : {
							'color' : '#ffffff'
						}
					},
					'vAxis' : {
						'title' : 'Humidity (%)',
						'titleTextStyle' : {
							'color' : '#ffffff'
						},
						'textStyle' : {
							'color' : '#ffffff'
						},
						// 'minValue' : 0,
						// 'maxValue' : 100,
						'format' : '#.##\'%\''
					}
				},
				'view' : {
					'columns' : [0,4]
				}
			});

			sgCharts.humControl = new google.visualization.ControlWrapper({
				'controlType' : 'ChartRangeFilter',
				'containerId' : 'sg-humidity-control',
				'options' : sgCharts.getControlOptions()
			});
			// bind controls to charts and draw dashboard
			sgCharts.dashboard.bind(sgCharts.o2Control, sgCharts.o2Chart);
			sgCharts.dashboard.bind(sgCharts.co2Control, sgCharts.co2Chart);
			sgCharts.dashboard.bind(sgCharts.tempControl, sgCharts.tempChart);
			sgCharts.dashboard.bind(sgCharts.humControl, sgCharts.humChart);
			sgCharts.dashboard.draw(sgCharts.dtDashboard);

			$('#sg-dashboard-div').show();
		});

	} // end setup dashbaord

}); // end doc ready

// ******** EXPECTED JSON FORMAT FOR DATATABLE (can pass directly to constructor)
// {
//   "cols": [
//         {"id":"","label":"Topping","pattern":"","type":"string"},
//         {"id":"","label":"Slices","pattern":"","type":"number"}
//       ],
//   "rows": [
//         {"c":[{"v":"Mushrooms","f":null},{"v":3,"f":null}]},
//         {"c":[{"v":"Onions","f":null},{"v":1,"f":null}]},
//         {"c":[{"v":"Olives","f":null},{"v":1,"f":null}]},
//         {"c":[{"v":"Zucchini","f":null},{"v":1,"f":null}]},
//         {"c":[{"v":"Pepperoni","f":null},{"v":2,"f":null}]}
//       ]
// }
// *****************************************************************************


// add stripslashes function for post responses
String.prototype.stripSlashes = function(){
    return this.replace(/\\(.)/mg, "$1");
}

// Load the Google Visualization API and necessary packages
google.charts.load('current', {'packages':['corechart', 'controls', 'table']});

/* Module for sg charts and underlying data */
var sgCharts = (function() {
	var activeChamber = null;
	var activeTable = true;
	var controlOptions = {
		'filterColumnIndex' : 0,
		'ui' : {
			// 'snapToData' : true,
			'chartOptions' : {
				'chartArea' : {
					'height' : 40
				},
				'colors' : [ '#ff0000' ],
				'hAxis' : {
					'textStyle' : {
						'bold' : true
					}
				}
			}
		}
	}
	var ctOptions = {
		width : '100%',
		height : '100%',
		sortColumn : 0
	}
	return {
		getActive : function() {
			return activeChamber;
		},
		setActive : function(chamberNumber) {
			activeChamber = chamberNumber;
		},
		// getIsActive : function() {
		// 	return activeTable;
		// },
		// setIsActive : function(isActive) {
		// 	activeTable = isActive;
		// },
		getControlOptions : function() {
			return controlOptions;
		},
		getCTOptions : function() {
			return ctOptions;
		},
		// google charts dashboard
		dashboard : {},
		// datatable for the dashbaord
		dtDashboard : {},
		// chamber table
		chamberTable : {},
		// datatable for active chamber table
		dtChamberTable : {},
		// datatable for chamber history table
		dtHistoryTable : {},
		// chart of O2 sensor readings
		o2Chart : {},
		// control for O2 chart
		o2Control : {},
		// chart for CO2 sensor readings
		co2Chart : {},
		// control for CO2 chart
		co2Control : {},
		// chart for temperature sensor readings
		tempChart : {},
		// control for temperature chart
		tempControl : {},
		// chart for humidity sensor readings
		humChart : {},
		// control for humidity chart
		humControl : {}
	};
})();
