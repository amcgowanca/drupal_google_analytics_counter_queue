<?php
/**
 * @file
 * GoogleAnalyticsCounterQueueGoogleAnalyticsOAuthClient.inc
 */

/**
 *
 */
class GoogleAnalyticsCounterQueueGoogleApiClient {
  /**
   * @var OAuthSignatureMethod_HMAC_SHA1
   */
  private $signature_method = NULL;

  /**
   * @var GoogleAnalyticsCounterQueueOAuthToken
   */
  private $consumer;

  /**
   * @var GoogleAnalyticsCounterQueueOAuthToken
   */
  private $token;

  /**
   * @var string
   */
  private $verifier;

  /**
   * Creates a new instance of GoogleAnalyticsCounterQueueAnalyticsOAuthClient.
   *
   * @access public
   *
   * @param GoogleAnalyticsCounterQueueOAuthToken $consumer
   *   The oauth consumer token object.
   * @param GoogleAnalyticsCounterQueueOAuthToken $token
   *   The oauth token object.
   */
  public function __construct(GoogleAnalyticsCounterQueueOAuthToken $consumer, GoogleAnalyticsCounterQueueOAuthToken $token = NULL) {
    $this->signature_method = new OAuthSignatureMethod_HMAC_SHA1();
    $this->consumer = $consumer;

    if (!empty($token)) {
      $this->token = $token;
    }
  }

  public function getConsumer() {
    return $this->consumer;
  }

  public function getSignatureMethod() {
    return $this->signature_method;
  }

  public function getVerifier() {
    return $this->verifier;
  }

  public function setVerifier($verifier) {
    $this->verifier = $verifier;
    return $this;
  }

  public function getToken() {
    return $this->token;
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
    return !empty($this->token);
  }

  /**
   * @return GoogleAnalyticsCounterQueueOAuthToken
   */
  public function getRequestToken() {
    if (empty($this->token)) {
      try {
        $result = GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication::getRequestToken($this);
        if (!$result->isEmpty()) {
          $data_str = $result->getData();
          parse_str($data_str, $data);
          $this->consumer = new GoogleAnalyticsCounterQueueOAuthToken($data['oauth_token'], $data['oauth_token_secret']);
        }
      }
      catch (Exception $exception) {

      }
    }

    return $this->getConsumer();
  }

  /**
   *
   */
  public function obtainAuthorization() {
    if (module_exists('overlay') && 'child' == overlay_get_mode()) {
      overlay_close_dialog();
      overlay_deliver_empty_page();
    }

    GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication::obtainAuthorization($this);
  }

  /**
   * @access public
   * @throws Exception
   */
  public function getAccessToken() {
    $result =  GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication::getAccessToken($this);
    if (!$result->isEmpty()) {
      $data_str = $result->getData();
      parse_str($data_str, $data);
      $this->token = new GoogleAnalyticsCounterQueueOAuthToken($data['oauth_token'], $data['oauth_token_secret']);
    }
    else {
      // TODO: Cleanup.
      throw new Exception(t('Unable to retrieve access token?'));
    }
    return $this->token;
  }

  /**
   * @access public
   */
  public function revokeToken() {
    GoogleAnalyticsCounterQueueGoogleAnalyticsAuthentication::revokeToken($this);
    variable_set('google_analytics_counter_queue_google_analytics_oauth_consumer_key', NULL);
    variable_set('google_analytics_counter_queue_google_analytics_oauth_consumer_secret', NULL);
    variable_set('google_analytics_counter_queue_google_analytics_oauth_token', NULL);
    variable_set('google_analytics_counter_queue_google_analytics_oauth_token_secret', NULL);
  }
}
