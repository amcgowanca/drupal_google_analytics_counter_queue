<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsApiPagePathViewsQuery.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleAnalyticsApiProfilesQuery
 */
class GoogleAnalyticsCounterQueueGoogleAnalyticsApiPagePathViewsQuery extends GoogleAnalyticsCounterQueueGoogleAnalyticsApiDataQuery {
  /**
   * {@inheritdoc}
   */
  public function getDefaultParameters() {
    return array(
      'ids' => array(),
      'dimensions' => array(),
      'metrics' => array(),
      'filters' => array(),
      'start-date' => '',
      'end-date' => '',
      'start-index' => 1,
      'max-results' => 50,
    );
  }
}
