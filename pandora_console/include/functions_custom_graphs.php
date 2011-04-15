<?php 

// Pandora FMS - http://pandorafms.com
// ==================================================
// Copyright (c) 2005-2011 Artica Soluciones Tecnologicas
// Please see http://pandorafms.org for full contribution list

// This program is free software; you can redistribute it and/or
// modify it under the terms of the  GNU Lesser General Public License
// as published by the Free Software Foundation; version 2

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

/**
 * @package Include
 * @subpackage Graphs
 */


/**
 * @global array Contents all var configs for the local instalation. 
 */ 
global $config;
if ($config['flash_charts']) {
	require_once ('include/fgraph.php');
}

require_once ($config["homedir"] . '/include/functions_graph.php');

/**
 * Get all the custom graphs a user can see.
 *
 * @param $id_user User id to check.
 * @param $only_names Wheter to return only graphs names in an associative array
 * or all the values.
 * @param $returnAllGroup Wheter to return graphs of group All or not.
 * @param $privileges Privileges to check in user group
 *
 * @return Custom graphs of a an user. Empty array if none.
 */
function custom_graphs_get_user ($id_user = 0, $only_names = false, $returnAllGroup = true, $privileges = 'IR') {
	global $config;
	
	if (!$id_user) {
		$id_user = $config['id_user'];
	}

 	$groups = get_user_groups ($id_user, $privileges, $returnAllGroup);

	$all_graphs = get_db_all_rows_in_table ('tgraph', 'name');
	if ($all_graphs === false)
		return array ();

	$graphs = array ();
	foreach ($all_graphs as $graph) {
		if (!in_array($graph['id_group'], array_keys($groups)))
			continue;

		if ($graph["id_user"] != $id_user && $graph['private'])
			continue;

		if ($graph["id_group"] > 0)
			if (!isset($groups[$graph["id_group"]])){
				continue;
			}

		if ($only_names) {
			$graphs[$graph['id_graph']] = $graph['name'];
		}
		else {
			$graphs[$graph['id_graph']] = $graph;
			$graphsCount = get_db_value_sql("SELECT COUNT(id_gs) FROM tgraph_source WHERE id_graph = " . $graph['id_graph']);
			$graphs[$graph['id_graph']]['graphs_count'] = $graphsCount;
		}
	}

	return $graphs;
}

/**
 * Print a custom graph image.
 *
 * @param $id_graph Graph id to print.
 * @param $height Height of the returning image.
 * @param $width Width of the returning image.
 * @param $period Period of time to get data in seconds.
 * @param $stacked Wheter the graph is stacked or not.
 * @param $return Whether to return an output string or echo now (optional, echo by default).
 * @param $date Date to start printing the graph
 */

function custom_graphs_print ($id_graph, $height, $width, $period, $stacked, $return = false, $date = 0) {
	global $config;

	$sources = get_db_all_rows_field_filter ('tgraph_source', 'id_graph', $id_graph);
	$modules = array ();
	$weights = array ();
	
	if($sources === false) {
		echo "<div class='nf'>".__('Empty graph')."</div>";
		return;
	}
	
	foreach ($sources as $source) {
		array_push ($modules, $source['id_agent_module']);
		array_push ($weights, $source['weight']);
	}
	
	$output = graphic_combined_module2($modules, $weights, $period, $width, $height,
		'', '', 0, 0, 0, $stacked, $date);

	/*
	if ($config['flash_charts']) {
		$output = graphic_combined_module ($modules, $weights, $period, $width, $height,
				'', '', 0, 0, 0, $stacked, $date);
	} else {
		$modules = implode (',', $modules);
		$weights = implode (',', $weights);
		$output = '<img src="include/fgraph.php?tipo=combined&height='.$height.'&width='.$width.'&id='.$modules.'&period='.$period.'&weight_l='.$weights.'&stacked='.$stacked.'&date='.$date.'">';
	}
	*/
	
	if ($return)
		return $output;
	echo $output;
}

/**
 * Get all the possible periods in a custom graph.
 *
 * @return The possible periods in a custom graph in an associative array.
 */
function custom_graphs_get_periods () {
	$periods = array ();
	
	$periods[1] = __('1 hour');
	$periods[2] = '2 '.__('hours');
	$periods[3] = '3 '.__('hours');
	$periods[6] = '6 '.__('hours');
	$periods[12] = '12 '.__('hours');
	$periods[24] = __('1 day');
	$periods[48] = __('2 days');
	$periods[360] = __('1 week');
	$periods[720] = __('1 month');
	$periods[4320] = __('6 months');
	
	return $periods;
}

?>
