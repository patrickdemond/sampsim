#!/usr/bin/php
<?php
/**
 * Script used to create true mean vs mse plots for all sampler, size and rr values
 */ 

// get a list of all conf and csv files
$command = 'ls links/ | sort -n | sed -e "s#.*#find links/&/*_sample -type f | grep \'\\\\.csv$\' | sort#" | /bin/bash';
$output = array();
exec( $command, $output );

$vals = array(
  'type' => array(),
  'pop' => array(),
  'sampler' => array(),
  'rr' => array(),
  'size' => array()
);

// load all conf and data file contents
$data_file_list = array();
foreach( $output as $file )
{
  $matches = array();
  preg_match( '#links/([0-9]+)/([a-z_]+)/([nr])(s[0-9]\.)?([0-9]+)\.csv#', $file, $matches );
  $type = $matches[3];
  if( !in_array( $type, $vals['type'] ) ) $vals['type'][] = $type;
  $pop = intval( $matches[1] );
  if( !in_array( $pop, $vals['pop'] ) ) $vals['pop'][] = $pop;
  $sampler = str_replace( '_sample', '', $matches[2] );
  if( 0 < strlen( $matches[4] ) ) $sampler = sprintf( '%s-%s', $sampler, substr( $matches[4], 0, -1 ) );
  if( !in_array( $sampler, $vals['sampler'] ) ) $vals['sampler'][] = $sampler;
  $size = intval( $matches[5] );
  if( !in_array( $size, $vals['size'] ) ) $vals['size'][] = $size;
  
  if( !array_key_exists( $type, $data_file_list ) )
    $data_file_list[$type] = array();
  if( !array_key_exists( $pop, $data_file_list[$type] ) )
    $data_file_list[$type][$pop] = array();
  if( !array_key_exists( $sampler, $data_file_list[$type][$pop] ) )
    $data_file_list[$type][$pop][$sampler] = array();
  if( !array_key_exists( $size, $data_file_list[$type][$pop][$sampler] ) )
    $data_file_list[$type][$pop][$sampler][$size] = array();

  $data_file_list[$type][$pop][$sampler][$size] = file_get_contents( $file );
}

// get all rr values from the data files
foreach( $data_file_list as $type => $pop_list )
{
  foreach( $pop_list as $pop => $sampler_list )
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
}

// setup mse_values array
$mse_values = array();
$mse_rr_values = array();
foreach( $vals['sampler'] as $sampler )
{
  $mse_values[$sampler] = array();
  $mse_rr_values[$sampler] = array();
  foreach( $vals['size'] as $size )
  {
    $mse_values[$sampler][$size] = array();
    $mse_rr_values[$sampler][$size] = array();
    foreach( $vals['rr'] as $rr )
    {
      $mse_values[$sampler][$size][$rr] = array();
      $mse_rr_values[$sampler][$size][$rr] = array();
      foreach( $vals['pop'] as $pop )
      {
        $mse_values[$sampler][$size][$rr][$pop] = array();
        $mse_rr_values[$sampler][$size][$rr][$pop] = array();
        foreach( $vals['type'] as $type )
        {
          $mse_values[$sampler][$size][$rr][$pop][$type] = '';
          $mse_rr_values[$sampler][$size][$rr][$pop][$type] = '';
        }
      }
    }
  }
}

// parse data files
$truerr_list = array();
$truemean_list = array();
foreach( $data_file_list as $type => $pop_list )
{
  foreach( $pop_list as $pop => $sampler_list )
  {
    $truerr_list[$pop] = array();
    $truemean_list[$pop] = array();
    foreach( $sampler_list as $sampler => $size_list )
    {
      foreach( $size_list as $size => $data_file )
      {
        $rr_stdev_list = array();
        $rr_list = array();
        $mean_list = array();
        $stdev_list = array();
        foreach( explode( "\n", $data_file ) as $index => $line )
        {
          if( 'population,child,' == substr( $line, 0, 17 ) )
          {
            $matches = array();
            preg_match( '#population,child,([0-9.]+),.*,([0-9.]+),([0-9.]+)$#', $line, $matches );
            $rr = strval( $matches[1] );
            if( !in_array( $rr, $vals['rr'] ) ) $vals['rr'][] = $rr;
            $truerr_list[$pop][$rr] = floatval( $matches[2] );
            $truemean_list[$pop][$rr] = floatval( $matches[3] );
          }
          else if( 'sample,child,' == substr( $line, 0, 13 ) )
          {
            $matches = array();
            preg_match( '#sample,child,([0-9.]+),.*,([0-9.]+),([0-9.]+),([0-9.]+),([0-9.]+)$#', $line, $matches );
            if( 5 < count( $matches ) )
            {
              $rr = strval( $matches[1] );
              $rr_stdev_list[$rr] = floatval( $matches[2] );
              $rr_list[$rr] = floatval( $matches[3] );
              $mean_list[$rr] = floatval( $matches[4] );
              $stdev_list[$rr] = floatval( $matches[5] );
            }
          }
        }

        foreach( array_keys( $truerr_list[$pop] ) as $rr )
        {
          if( array_key_exists( $rr, $mean_list ) && array_key_exists( $rr, $stdev_list ) )
            $mse_values[$sampler][$size][$rr][$pop][$type] = 
              ($truemean_list[$pop][$rr] - $mean_list[$rr])*($truemean_list[$pop][$rr] - $mean_list[$rr]) +
              $stdev_list[$rr]*$stdev_list[$rr];
          if( array_key_exists( $rr, $rr_list ) && array_key_exists( $rr, $rr_stdev_list ) )
            $mse_rr_values[$sampler][$size][$rr][$pop][$type] = 
              ($truerr_list[$pop][$rr] - $rr_list[$rr])*($truerr_list[$pop][$rr] - $rr_list[$rr]) +
              $rr_stdev_list[$rr]*$rr_stdev_list[$rr];
        }
      }
    }
  }
}

// print plot data and create plots
if( file_exists( 'plots' ) ) exec( 'rm -rf plots' );
mkdir( 'plots' );
foreach( $vals['type'] as $type )
{
  foreach( $vals['rr'] as $rr )
  {
    foreach( $vals['size'] as $size )
    {
      // prevalence: true vs mse
      //////////////////////////////////////////////////////////////////////////////////////////////////////////
      $filename = sprintf( 'plots/prevalence_%s_%0.1f_%d', $type, $rr, $size );
      $title = sprintf(
        'Prevalence (True vs MSE): %s, RR=%0.1f, size=%d',
        'n' == $type ? 'Towns not resampled' : 'Towns resampled',
        $rr,
        $size
      );

      // heading
      $data = 'Prevalence';
      foreach( $vals['sampler'] as $sampler ) $data .= ','.ucwords( str_replace( '_', ' ', $sampler ) );
      $data .= "\n";

      // print data
      foreach( $vals['pop'] as $pop )
      {
        $data .= $truemean_list[$pop][$rr];
        foreach( $vals['sampler'] as $sampler ) $data .= ','.$mse_values[$sampler][$size][$rr][$pop][$type];
        $data .= "\n";
      }

      file_put_contents( sprintf( '%s.csv', $filename ), $data );

      $plot = sprintf(
        'set title "%s" font "sans, 18"; '.
        'set datafile separator ","; '.
        'set terminal png size 1400,1000; '.
        'set output "%s.png"; '.
        'plot "%s.csv" using 1:2 title columnheader(2), '.
             '"%s.csv" using 1:3 title columnheader(3), '.
             '"%s.csv" using 1:4 title columnheader(4), '.
             '"%s.csv" using 1:5 title columnheader(5);',
        $title,
        $filename, $filename, $filename, $filename, $filename
      );
      exec( sprintf( "gnuplot -e '%s'", $plot ) );

      // relative risk: true vs mse
      //////////////////////////////////////////////////////////////////////////////////////////////////////////
      $filename = sprintf( 'plots/rr_%s_%0.1f_%d', $type, $rr, $size );
      $title = sprintf(
        'Relative Risk (True vs MSE): %s, RR=%0.1f, size=%d',
        'n' == $type ? 'Towns not resampled' : 'Towns resampled',
        $rr,
        $size
      );

      // heading
      $data = 'RR';
      foreach( $vals['sampler'] as $sampler ) $data .= ','.ucwords( str_replace( '_', ' ', $sampler ) );
      $data .= "\n";

      // print data
      foreach( $vals['pop'] as $pop )
      {
        $data .= $truerr_list[$pop][$rr];
        foreach( $vals['sampler'] as $sampler ) $data .= ','.$mse_rr_values[$sampler][$size][$rr][$pop][$type];
        $data .= "\n";
      }

      file_put_contents( sprintf( '%s.csv', $filename ), $data );

      $plot = sprintf(
        'set title "%s" font "sans, 18"; '.
        'set datafile separator ","; '.
        'set terminal png size 1400,1000; '.
        'set output "%s.png"; '.
        'plot "%s.csv" using 1:2 title columnheader(2), '.
             '"%s.csv" using 1:3 title columnheader(3), '.
             '"%s.csv" using 1:4 title columnheader(4), '.
             '"%s.csv" using 1:5 title columnheader(5);',
        $title,
        $filename, $filename, $filename, $filename, $filename
      );
      exec( sprintf( "gnuplot -e '%s'", $plot ) );
    }
  }
}

/*
foreach( $vals['param'] as $param )
{
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
        $data .= $param_values[$param][$pop];
        foreach( $vals['sampler'] as $sampler ) $data .= ','.$mse_values[$sampler][$size][$rr][$pop];
        $data .= "\n";
      }

      file_put_contents( $filename.'.csv', $data );
      $plot = sprintf(
        'set title "%s" font "sans, 18"; '.
        'set datafile separator ","; '.
        'set terminal png size 1400,1000; '.
        'set output "%s.png"; '.
        'plot "%s.csv" using 1:2 title columnheader(2), '.
             '"%s.csv" using 1:3 title columnheader(3), '.
             '"%s.csv" using 1:4 title columnheader(4), '.
             '"%s.csv" using 1:5 title columnheader(5);',
        sprintf( '%s (ss=%d,rr=%0.1f)', ucwords( str_replace( '_', ' ', $param ) ), $size, $rr ),
        $filename, $filename, $filename, $filename, $filename
      );
      exec( sprintf( "gnuplot -e '%s'", $plot ) );
    }
  }
}
*/
