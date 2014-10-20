<?php
/**
 * @file
 */

class GoogleAnalyticsCounterQueueGoogleAnalyticsApiFilterableFilterableQueryParameterOperatorEquals extends GoogleAnalyticsCounterQueueGoogleAnalyticsApiFilterableQueryParameterOperator {
  public function __construct() {
    parent::__construct('==');
  }
}