#!/usr/bin/php
# 
# PHP script used to create parameter vs mse plots for all sampler, size and rr values
# 
<?php

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
    $index = intval( $matches[1] );
    $vals['pop'][] = $index;
    $conf_file_list[$index] = file_get_contents( $file );
  }
  else if( false !== strpos( $file, '.csv' ) )
  {
    $matches = array();
    preg_match( '#links/([0-9]+)/([a-z_]+)/r(s[0-9]\.)?([0-9]+)\.csv#', $file, $matches );
    $index = intval( $matches[1] );
    $sampler = str_replace( '_sample', '', $matches[2] );
    if( 0 < strlen( $matches[3] ) ) $sampler = sprintf( '%s-%s', $sampler, substr( $matches[3], 0, -1 ) );
    if( !in_array( $sampler, $vals['sampler'] ) ) $vals['sampler'][] = $sampler;
    $size = intval( $matches[4] );
    if( !in_array( $size, $vals['size'] ) ) $vals['size'][] = $size;
    
    if( !array_key_exists( $index, $data_file_list ) )
      $data_file_list[$index] = array();
    if( !array_key_exists( $sampler, $data_file_list[$index] ) )
      $data_file_list[$index][$sampler] = array();
    if( !array_key_exists( $size, $data_file_list[$index][$sampler] ) )
      $data_file_list[$index][$sampler][$size] = array();

    $data_file_list[$index][$sampler][$size] = file_get_contents( $file );
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
    foreach( $vals['rr'] as $rr ) $mse_values[$sampler][$size][$rr] = array();
  }
}

foreach( $data_file_list as $index => $sampler_list )
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
        $mse_values[$sampler][$size][$rr][] = 
          ($truemean - $mean_list[$rr])*($truemean - $mean_list[$rr]) + $stdev_list[$rr]*$stdev_list[$rr];
    }
  }
}

// print plot data
foreach( $vals['param'] as $param_index => $param_name )
{
  foreach( $vals['size'] as $size_index => $size_name )
  {
    foreach( $vals['rr'] as $rr_index => $rr_name )
    {
      // print title
      printf( "%s (ss=%d,rr=%0.1f)\n", $param_name, $size_name, $rr_name );
      // print heading
      print 'param';
      foreach( $vals['sampler'] as $sampler ) print ','.$sampler;
      print "\n";

      // print data
      foreach( $vals['pop'] as $pop_index => $pop_name )
      {
        print $param_values[$param_name][$pop_index];
        foreach( $vals['sampler'] as $sampler )
        {
          print ',';
          print in_array( $sampler, $mse_values ) &&
                in_array( $size_name, $mse_values[$sampler] ) &&
                in_array( $rr_name, $mse_values[$sampler][$size_name] )
              ? $mse_values[$sampler][$size_name][$rr_name]
              : '';
        }
        print "\n";
      }

      print "\n";
    }
  }
}
