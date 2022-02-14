#!/usr/bin/perl

$strStart = "SIMULATIONSIMULATION";

$filename = @ARGV[0];

$plotdatafn = @ARGV[1];

open(IN, "$filename");
open(OUT, ">$plotdatafn");

$onus = 0;
$load = 0;
$state = 0;
$time1 = 0;
$time2 = 0;
$curOnus = 0;


while(<IN>)
{
  $trace = $_;

  chomp $trace;
  if($trace eq $strStart) 
  {
    $state = 0;
  } 
  else
  {
    $state = $state + 1;
  }

  if($state == 1)
  {
    @items = split /-/, $trace;

    $curOnus = @items[1];
    $rate = @items[2] + 0.0;
    $load = $curOnus * $rate;
  }
  elsif($state==2)
  {
    @tmpitems = split /\s+/, $trace;
    @items = split /:/, @tmpitems[3];
    $hour = @items[0] + 0;
    $min = @items[1] + 0;
    $second = @items[2] + 0;
    $time1 = ($hour * 3600) + ($min * 60) + $second;
    #print OUT "$hour $min $second $time1\n";
  }
  elsif($state==3)
  {
    @tmpitems = split /\s+/, $trace;
    @items = split /:/, @tmpitems[3];
    $hour = @items[0] + 0;
    $min = @items[1] + 0;
    $second = @items[2] + 0;
    $time2 = ($hour * 3600) + ($min * 60) + $second;
    #print OUT "$hour $min $second $time2\n";

    if($curOnus != $onus)
    {
      print OUT "\n";
      $onus = $curOnus;
    }

    $diff = $time2 - $time1;
    if($time2 < $time1) 
    {
      $diff = $diff + 86400;
    }
    $diff = $diff / 400;

    print OUT "$curOnus $load $diff \n";
  }
}



close(IN);
close(OUT);
