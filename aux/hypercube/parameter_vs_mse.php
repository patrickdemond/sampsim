#!/usr/bin/php
<?php
/**
 * Script used to create parameter vs mse plots for all sampler, size and rr values
 */ 

// get a list of all conf and csv files
$command = 'ls links/ | sort -n | sed -e "s#.*#find links/&/ -type f | grep -v \'sample/n\' | sort#" | /bin/bash';
$output = array();
exec( $command, $output );

$vals = array(
  'pop' => array(),
  'sampler' => array(),
  'rr' => array(),
  'size' => array(),
  'param' => array()
);

// load all conf and data file contents
$conf_file_list = array();
$data_file_list = array();
foreach( $output as $file )
{
  if( false !== strpos( $file, '.conf' ) )
  {
    $matches = array();
    preg_match( '#links/([0-9]+)/#', $file, $matches );
    $pop = intval( $matches[1] );
    $vals['pop'][] = $pop;
    $conf_file_list[$pop] = file_get_contents( $file );
  }
  else if( false !== strpos( $file, '.csv' ) )
  {
    $matches = array();
    preg_match( '#links/([0-9]+)/([a-z_]+)/r(s[0-9]\.)?([0-9]+)\.csv#', $file, $matches );
    $pop = intval( $matches[1] );
    $sampler = str_replace( '_sample', '', $matches[2] );
    if( 0 < strlen( $matches[3] ) ) $sampler = sprintf( '%s-%s', $sampler, substr( $matches[3], 0, -1 ) );
    if( !in_array( $sampler, $vals['sampler'] ) ) $vals['sampler'][] = $sampler;
    $size = intval( $matches[4] );
    if( !in_array( $size, $vals['size'] ) ) $vals['size'][] = $size;
    
    if( !array_key_exists( $pop, $data_file_list ) )
      $data_file_list[$pop] = array();
    if( !array_key_exists( $sampler, $data_file_list[$pop] ) )
      $data_file_list[$pop][$sampler] = array();
    if( !array_key_exists( $size, $data_file_list[$pop][$sampler] ) )
      $data_file_list[$pop][$sampler][$size] = array();

    $data_file_list[$pop][$sampler][$size] = file_get_contents( $file );
  }
}

// get all rr values from the data files
foreach( $data_file_list as $pop => $sampler_list )
{
  foreach( $sampler_list as $sampler => $size_list )
  {
    foreach( $size_list as $size => $data_file )
    {
      foreach( explode( "\n", $data_file ) as $line )
      {
        if( 'population,child,' == substr( $line, 0, 17 ) )
        {
          $matches = array();
          preg_match( '#population,child,([0-9.]+),.*,([0-9.]+)$#', $line, $matches );
          $rr = strval( $matches[1] );
          if( !in_array( $rr, $vals['rr'] ) ) $vals['rr'][] = $rr;
        }
      }
    }
  }
}

// parse conf parameter values
$param_values = array();
foreach( $conf_file_list as $conf_file )
{
  foreach( explode( "\n", $conf_file ) as $line )
  {
    $line = trim( preg_replace( '/#.*/', '', $line ) );
    if( 0 < strlen( $line ) )
    {
      $parts = explode( ': ', $line );
      $name = $parts[0];
      $value = $parts[1];
      if( !array_key_exists( $name, $param_values ) ) $param_values[$name] = array();
      $param_values[$name][] = $value;
    }
  }
}

// drop constant parameters
foreach( $param_values as $name => $values )
  if( 1 == count( array_unique( $values ) ) )
    unset( $param_values[$name] );
$vals['param'] = array_keys( $param_values );

// parse data mean and stdev values
$mse_values = array();
foreach( $vals['sampler'] as $sampler )
{
  $mse_values[$sampler] = array();
  foreach( $vals['size'] as $size )
  {
    $mse_values[$sampler][$size] = array();
    foreach( $vals['rr'] as $rr )
    {
      $mse_values[$sampler][$size][$rr] = array();
      foreach( $vals['pop'] as $pop ) $mse_values[$sampler][$size][$rr][$pop] = '';
    }
  }
}

foreach( $data_file_list as $pop => $sampler_list )
{
  foreach( $sampler_list as $sampler => $size_list )
  {
    foreach( $size_list as $size => $data_file )
    {
      $truemean_list = array();
      $mean_list = array();
      $stdev_list = array();
      foreach( explode( "\n", $data_file ) as $line )
      {
        if( 'population,child,' == substr( $line, 0, 17 ) )
        {
          $matches = array();
          preg_match( '#population,child,([0-9.]+),.*,([0-9.]+)$#', $line, $matches );
          $rr = strval( $matches[1] );
          if( !in_array( $rr, $vals['rr'] ) ) $vals['rr'][] = $rr;
          $truemean_list[$rr] = floatval( $matches[2] );
        }
        else if( 'sample,child,' == substr( $line, 0, 13 ) )
        {
          $matches = array();
          preg_match( '#sample,child,([0-9.]+),.*,([0-9.]+),([0-9.]+)$#', $line, $matches );
          $rr = strval( $matches[1] );
          $mean_list[$rr] = floatval( $matches[2] );
          $stdev_list[$rr] = floatval( $matches[3] );
        }
      }

      foreach( $truemean_list as $rr => $truemean )
        $mse_values[$sampler][$size][$rr][$pop] = 
          ($truemean - $mean_list[$rr])*($truemean - $mean_list[$rr]) + $stdev_list[$rr]*$stdev_list[$rr];
    }
  }
}

// print plot data and create plots
if( file_exists( 'plots' ) ) exec( 'rm -rf plots' );
mkdir( 'plots' );
foreach( $vals['param'] as $param )
{
  $categorical = 1 === preg_match( '/[^0-9.]/', $param_values[$param][0] );
  if( $categorical )
  {
    $categories = array_unique( array_values( $param_values[$param] ) );
    sort( $categories );
    $xtics = '';
    foreach( $categories as $index => $category )
      $xtics .= sprintf( '%s"%s" %d', 0 < $index ? ',' : '', $category, $index );
    $category_plot = sprintf( 'set xrange [-1:%d]; set xtics(%s); ', count( $categories ), $xtics );
    if( 8 < count( $categories ) ) $category_plot .= 'set xtics rotate; ';
  }
  $dir = sprintf( 'plots/%s', $param );
  if( !file_exists( $dir ) ) mkdir( $dir );
  foreach( $vals['size'] as $size )
  {
    foreach( $vals['rr'] as $rr )
    {
      $filename = sprintf( '%s/%0.1f-%d', $dir, $rr, $size );

      // heading
      $data = 'Param';
      foreach( $vals['sampler'] as $sampler ) $data .= ','.ucwords( str_replace( '_', ' ', $sampler ) );
      $data .= "\n";

      // print data
      foreach( $vals['pop'] as $pop )
      {
        $data .= $categorical
               ? array_search( $param_values[$param][$pop], $categories )
               : $param_values[$param][$pop];
        foreach( $vals['sampler'] as $sampler ) $data .= ','.$mse_values[$sampler][$size][$rr][$pop];
        $data .= "\n";
      }

      file_put_contents( $filename.'.csv', $data );
      $plot = sprintf(
        'set title "%s" font "sans, 18"; '.
        'set datafile separator ","; '.
        'set terminal png size 1400,1000; '.
        '%s'.
        'set output "%s.png"; '.
        'plot "%s.csv" using 1:2 title columnheader(2), '.
             '"%s.csv" using 1:3 title columnheader(3), '.
             '"%s.csv" using 1:4 title columnheader(4), '.
             '"%s.csv" using 1:5 title columnheader(5);',
        sprintf( '%s (ss=%d,rr=%0.1f)', ucwords( str_replace( '_', ' ', $param ) ), $size, $rr ),
        $categorical ? $category_plot : '',
        $filename, $filename, $filename, $filename, $filename
      );
      exec( sprintf( "gnuplot -e '%s'", $plot ) );
    }
  }
}
