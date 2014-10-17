<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsApiQuery.inc
 */

/**
 * Class GoogleAnalyticsCounterQueueGoogleAnalyticsApiQuery
 */
class GoogleAnalyticsCounterQueueGoogleAnalyticsApiQuery extends GoogleAnalyticsCounterQueueGoogleApiQuery {
  /**
   * {@inheritdoc}
   */
  final public function getEndpointHost() {
    return 'https://www.googleapis.com/analytics/v3';
  }

  /**
   * {@inheritdoc}
   */
  final protected function setEndpointPath($path) {
    throw new Exception(t('Method setEndpointPath is not implemented and should not be invoked.'));
  }

  /**
   * {@inheritdoc}
   */
  protected function getResultsClassName() {
    return 'GoogleAnalyticsCounterQueueGoogleApiJsonResults';
  }
}
