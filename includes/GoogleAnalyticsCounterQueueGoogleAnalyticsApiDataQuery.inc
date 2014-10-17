<?php
/**
 * @file
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleAnalyticsApiDataQuery
 */
abstract class GoogleAnalyticsCounterQueueGoogleAnalyticsApiDataQuery extends GoogleAnalyticsCounterQueueGoogleAnalyticsApiQuery {
  /**
   * {@inheritdoc{
   */
  public function getEndpointPath() {
    return 'data/ga';
  }
}
