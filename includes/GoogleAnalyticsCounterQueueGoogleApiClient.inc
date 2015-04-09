<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsOAuthClient.inc
 */

/**
 *
 */
class GoogleAnalyticsCounterQueueGoogleApiClient {
  private $access_token = NULL;
  private $refresh_token = NULL;
  private $token_type = NULL;
  private $token_expires = NULL;

  /**
   * Creates a new instance of GoogleAnalyticsCounterQueueAnalyticsOAuthClient.
   *
   * @access public
   */
  public function __construct() {
    $this->access_token = variable_get('google_analytics_counter_queue_client_access_token', NULL);
    $this->refresh_token = variable_get('google_analytics_counter_queue_client_refresh_token', NULL);
    $this->token_type = variable_get('google_analytics_counter_queue_client_token_type', NULL);
    $this->token_expires = (int) variable_get('google_analytics_counter_queue_client_token_expires', 0);
  }

  /**
   * Checks if this instance is currently authenticated.
   *
   * @access public
   *
   * @return bool
   *   Returns TRUE if authenticated, otherwise FALSE.
   */
  public function isAuthenticated() {
    return !empty($this->access_token) ? TRUE : FALSE;
  }

  public function hasTokenExpired() {
    return REQUEST_TIME > $this->token_expires ? TRUE : FALSE;
  }

  public function setAccessToken($token) {
    $this->access_token = $token;
    variable_set('google_analytics_counter_queue_client_access_token', $this->access_token);
  }

  public function setRefreshToken($token) {
    $this->refresh_token = $token;
    variable_set('google_analytics_counter_queue_client_refresh_token', $this->refresh_token);
  }

  public function setTokenType($type) {
    $this->token_type = $type;
    variable_set('google_analytics_counter_queue_client_token_type', $this->token_type);
  }

  public function setTokenExpires($seconds, $from = REQUEST_TIME) {
    $this->token_expires = $from + $seconds;
    variable_set('google_analytics_counter_queue_client_token_expires', $this->token_expires);
  }

  public function getAccessToken() {
    return $this->access_token;
  }

  public function getRefreshToken() {
    return $this->refresh_token;
  }

  public function getTokenType() {
    return $this->token_type;
  }

  public function getTokenExpires() {
    return $this->token_expires;
  }

  public function obtainAuthorization() {
    google_analytics_counter_queue_api_auth_obtain_auth($this);
  }

  public function revokeToken() {
    try {
      $response = google_analytics_counter_queue_api_auth_revoke($this);
    }
    catch (Exception $exception) {
      google_analytics_counter_queue_log_exception($exception);
    }

    $this->setAccessToken(NULL);
    $this->setRefreshToken(NULL);
    $this->setTokenType(NULL);
    $this->setTokenExpires(NULL);

    // Notify other modules that authentication has been revoked.
    google_analytics_counter_queue_module_include();
    module_invoke_all('google_analytics_counter_queue_api_revoked');
  }
}
