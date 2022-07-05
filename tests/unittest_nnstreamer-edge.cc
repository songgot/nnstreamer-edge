/**
 * @file        unittest_nnstreamer-edge.cc
 * @date        27 June 2022
 * @brief       Unittest for nnstreamer-edge library.
 * @see         https://github.com/nnstreamer/nnstreamer-edge
 * @author      Jaeyun Jung <jy1210.jung@samsung.com>
 * @bug         No known bugs
 */

#include <gtest/gtest.h>
#include "nnstreamer-edge.h"
#include "nnstreamer-edge-common.h"
#include "nnstreamer-edge-internal.h"

/**
 * @brief Data struct for event callback test.
 */
typedef struct
{
  bool callback_released;
} ne_event_cb_test_s;

/**
 * @brief Edge event callback for test.
 */
static int
_test_edge_event_cb (nns_edge_event_h event_h, void *user_data)
{
  nns_edge_event_e event = NNS_EDGE_EVENT_UNKNOWN;
  ne_event_cb_test_s *event_data = (ne_event_cb_test_s *) user_data;

  if (!event_data) {
    /* Cannot update event status. */
    return NNS_EDGE_ERROR_NONE;
  }

  nns_edge_event_get_type (event_h, &event);

  switch (event) {
    case NNS_EDGE_EVENT_CALLBACK_RELEASED:
      event_data->callback_released = true;
      break;
    default:
      break;
  }

  return NNS_EDGE_ERROR_NONE;
}

/**
 * @brief Create edge handle - invalid param.
 */
TEST(edge, createHandleInvalidParam01_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle (NULL, "temp-topic", &edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge handle - invalid param.
 */
TEST(edge, createHandleInvalidParam02_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", NULL, &edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge handle - invalid param.
 */
TEST(edge, createHandleInvalidParam03_n) {
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge handle - invalid param.
 */
TEST(edge, createHandleInvalidParam04_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("", "temp-topic", &edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge handle - invalid param.
 */
TEST(edge, createHandleInvalidParam05_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "", &edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Start - invalid param.
 */
TEST(edge, startInvalidParam01_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_start (NULL, false);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Start - invalid param.
 */
TEST(edge, startInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_start (edge_h, false);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Release edge handle - invalid param.
 */
TEST(edge, releaseHandleInvalidParam01_n) {
  int ret;

  ret = nns_edge_release_handle (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Release edge handle - invalid param.
 */
TEST(edge, releaseHandleInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set event callback - null param to clear event callback.
 */
TEST(edge, setEventCbSetNullCallback) {
  nns_edge_h edge_h;
  ne_event_cb_test_s *event_data;
  int ret;

  event_data = (ne_event_cb_test_s *) malloc (sizeof (ne_event_cb_test_s));
  memset (event_data, 0, sizeof (ne_event_cb_test_s));

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_event_callback (edge_h, _test_edge_event_cb, event_data);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  /* Set null param to clear event callback. */
  ret = nns_edge_set_event_callback (edge_h, NULL, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  EXPECT_TRUE (event_data->callback_released);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (event_data);
}

/**
 * @brief Set event callback - invalid param.
 */
TEST(edge, setEventCbInvalidParam01_n) {
  int ret;

  ret = nns_edge_set_event_callback (NULL, _test_edge_event_cb, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set event callback - invalid param.
 */
TEST(edge, setEventCbInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  ne_event_cb_test_s *event_data;
  int ret;

  event_data = (ne_event_cb_test_s *) malloc (sizeof (ne_event_cb_test_s));
  memset (event_data, 0, sizeof (ne_event_cb_test_s));

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_set_event_callback (edge_h, _test_edge_event_cb, event_data);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (event_data);
}

/**
 * @brief Connect - invalid param.
 */
TEST(edge, connectInvalidParam01_n) {
  int ret;

  ret = nns_edge_connect (NULL, NNS_EDGE_PROTOCOL_TCP, "127.0.0.1", 80);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Connect - invalid param.
 */
TEST(edge, connectInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_event_callback (edge_h, _test_edge_event_cb, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_connect (edge_h, NNS_EDGE_PROTOCOL_TCP, "127.0.0.1", 80);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Connect - invalid param.
 */
TEST(edge, connectInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_event_callback (edge_h, _test_edge_event_cb, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_connect (edge_h, NNS_EDGE_PROTOCOL_TCP, NULL, 80);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Connect - invalid param.
 */
TEST(edge, connectInvalidParam04_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_event_callback (edge_h, _test_edge_event_cb, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_connect (edge_h, NNS_EDGE_PROTOCOL_TCP, "", 80);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Disconnect - invalid param.
 */
TEST(edge, disconnectInvalidParam01_n) {
  int ret;

  ret = nns_edge_disconnect (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Disconnect - invalid param.
 */
TEST(edge, disconnectInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_disconnect (edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Publish - invalid param.
 */
TEST(edge, publishInvalidParam01_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_publish (NULL, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Publish - invalid param.
 */
TEST(edge, publishInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_data_h data_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_publish (edge_h, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Publish - invalid param.
 */
TEST(edge, publishInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_publish (edge_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Request - invalid param.
 */
TEST(edge, requestInvalidParam01_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_request (NULL, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Request - invalid param.
 */
TEST(edge, requestInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_data_h data_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_request (edge_h, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Request - invalid param.
 */
TEST(edge, requestInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_request (edge_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Respond - invalid param.
 */
TEST(edge, respondInvalidParam01_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "client_id", "10");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_respond (NULL, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Respond - invalid param.
 */
TEST(edge, respondInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_data_h data_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "client_id", "10");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_respond (edge_h, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Respond - invalid param.
 */
TEST(edge, respondInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_respond (edge_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Respond - invalid param.
 */
TEST(edge, respondInvalidParam04_n) {
  nns_edge_h edge_h;
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  /* No client ID */
  ret = nns_edge_respond (edge_h, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Subscribe - invalid param.
 */
TEST(edge, subscribeInvalidParam01_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_subscribe (NULL, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Subscribe - invalid param.
 */
TEST(edge, subscribeInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_data_h data_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_subscribe (edge_h, data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Subscribe - invalid param.
 */
TEST(edge, subscribeInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_subscribe (edge_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Unsubscribe - invalid param.
 */
TEST(edge, unsubscribeInvalidParam01_n) {
  int ret;

  ret = nns_edge_unsubscribe (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Unsubscribe - invalid param.
 */
TEST(edge, unsubscribeInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_unsubscribe (edge_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get topic.
 */
TEST(edge, getTopic) {
  nns_edge_h edge_h;
  char *topic = NULL;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_get_topic (edge_h, &topic);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_STREQ (topic, "temp-topic");
  free (topic);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get topic - invalid param.
 */
TEST(edge, getTopicInvalidParam01_n) {
  char *topic = NULL;
  int ret;

  ret = nns_edge_get_topic (NULL, &topic);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get topic - invalid param.
 */
TEST(edge, getTopicInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  char *topic = NULL;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_get_topic (edge_h, &topic);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get topic - invalid param.
 */
TEST(edge, getTopicInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_get_topic (edge_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info - invalid param.
 */
TEST(edge, setInfoInvalidParam01_n) {
  int ret;

  ret = nns_edge_set_info (NULL, "topic", "temp-topic");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info - invalid param.
 */
TEST(edge, setInfoInvalidParam02_n) {
  nns_edge_h edge_h;
  nns_edge_handle_s *eh;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  eh = (nns_edge_handle_s *) edge_h;
  eh->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_set_info (edge_h, "topic", "temp-topic");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  eh->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info - invalid param.
 */
TEST(edge, setInfoInvalidParam03_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_info (edge_h, NULL, "temp-topic");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info - invalid param.
 */
TEST(edge, setInfoInvalidParam04_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_info (edge_h, "", "temp-topic");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info - invalid param.
 */
TEST(edge, setInfoInvalidParam05_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_info (edge_h, "topic", NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info - invalid param.
 */
TEST(edge, setInfoInvalidParam06_n) {
  nns_edge_h edge_h;
  int ret;

  ret = nns_edge_create_handle ("temp-id", "temp-topic", &edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_set_info (edge_h, "topic", "");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_release_handle (edge_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge-data - invalid param.
 */
TEST(edgeData, createInvalidParam01_n) {
  int ret;

  ret = nns_edge_data_create (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Destroy edge-data - invalid param.
 */
TEST(edgeData, destroyInvalidParam01_n) {
  int ret;

  ret = nns_edge_data_destroy (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Destroy edge-data - invalid param.
 */
TEST(edgeData, destroyInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Validate edge-data.
 */
TEST(edgeData, validate) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_is_valid (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Validate edge-data - invalid param.
 */
TEST(edgeData, validateInvalidParam01_n) {
  int ret;

  ret = nns_edge_data_is_valid (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Validate edge-data - invalid param.
 */
TEST(edgeData, validateInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_is_valid (data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Copy edge-data.
 */
TEST(edgeData, copy) {
  nns_edge_data_h src_h, desc_h;
  void *data, *result;
  size_t data_len, result_len;
  char *result_value;
  unsigned int i, result_count;
  int ret;

  data_len = 10U * sizeof (unsigned int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  for (i = 0; i < 10U; i++)
    ((unsigned int *) data)[i] = i;

  ret = nns_edge_data_create (&src_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (src_h, "temp-key1", "temp-data-val1");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  ret = nns_edge_data_set_info (src_h, "temp-key2", "temp-data-val2");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (src_h, data, data_len, free);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_copy (src_h, &desc_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (src_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  /* Compare data and info */
  ret = nns_edge_data_get_count (desc_h, &result_count);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_EQ (result_count, 1U);

  ret = nns_edge_data_get (desc_h, 0, &result, &result_len);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  for (i = 0; i < 10U; i++)
    EXPECT_EQ (((unsigned int *) result)[i], i);

  ret = nns_edge_data_get_info (desc_h, "temp-key1", &result_value);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_STREQ (result_value, "temp-data-val1");
  free (result_value);

  ret = nns_edge_data_get_info (desc_h, "temp-key2", &result_value);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_STREQ (result_value, "temp-data-val2");
  free (result_value);

  ret = nns_edge_data_destroy (desc_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Copy edge-data - invalid param.
 */
TEST(edgeData, copyInvalidParam01_n) {
  nns_edge_data_h desc_h;
  int ret;

  ret = nns_edge_data_copy (NULL, &desc_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Copy edge-data - invalid param.
 */
TEST(edgeData, copyInvalidParam02_n) {
  nns_edge_data_h src_h, desc_h;
  nns_edge_data_s *ed;
  int ret;

  ret = nns_edge_data_create (&src_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) src_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_copy (src_h, &desc_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (src_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Copy edge-data - invalid param.
 */
TEST(edgeData, copyInvalidParam03_n) {
  nns_edge_data_h src_h;
  int ret;

  ret = nns_edge_data_create (&src_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_copy (src_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (src_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Add edge-data - max data limit.
 */
TEST(edgeData, addMaxData_n) {
  nns_edge_data_h data_h;
  void *data;
  size_t data_len;
  int i, ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  for (i = 0; i < NNS_EDGE_DATA_LIMIT; i++) {
    ret = nns_edge_data_add (data_h, data, data_len, NULL);
    EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  }

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Add edge-data - invalid param.
 */
TEST(edgeData, addInvalidParam01_n) {
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_add (NULL, data, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Add edge-data - invalid param.
 */
TEST(edgeData, addInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Add edge-data - invalid param.
 */
TEST(edgeData, addInvalidParam03_n) {
  nns_edge_data_h data_h;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, NULL, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Add edge-data - invalid param.
 */
TEST(edgeData, addInvalidParam04_n) {
  nns_edge_data_h data_h;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, 0, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get edge-data.
 */
TEST(edgeData, get) {
  nns_edge_data_h data_h;
  void *data, *result;
  size_t data_len, result_len;
  unsigned int count;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get_count (data_h, &count);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_EQ (count, 1U);

  ret = nns_edge_data_get (data_h, 0, &result, &result_len);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_EQ (result, data);
  EXPECT_EQ (result_len, data_len);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get edge-data - invalid param.
 */
TEST(edgeData, getInvalidParam01_n) {
  void *data;
  size_t data_len;
  int ret;

  ret = nns_edge_data_get (NULL, 0, &data, &data_len);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get edge-data - invalid param.
 */
TEST(edgeData, getInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  void *data, *result;
  size_t data_len, result_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_get (data_h, 0, &result, &result_len);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get edge-data - invalid param.
 */
TEST(edgeData, getInvalidParam03_n) {
  nns_edge_data_h data_h;
  void *data, *result;
  size_t data_len, result_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  /* Invalid index */
  ret = nns_edge_data_get (data_h, 1, &result, &result_len);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get edge-data - invalid param.
 */
TEST(edgeData, getInvalidParam04_n) {
  nns_edge_data_h data_h;
  void *data;
  size_t data_len, result_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get (data_h, 0, NULL, &result_len);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get edge-data - invalid param.
 */
TEST(edgeData, getInvalidParam05_n) {
  nns_edge_data_h data_h;
  void *data, *result;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get (data_h, 0, &result, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get count of edge-data - invalid param.
 */
TEST(edgeData, getCountInvalidParam01_n) {
  unsigned int count;
  int ret;

  ret = nns_edge_data_get_count (NULL, &count);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get count of edge-data - invalid param.
 */
TEST(edgeData, getCountInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_get_count (data_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get count of edge-data - invalid param.
 */
TEST(edgeData, getCountInvalidParam03_n) {
  nns_edge_data_h data_h;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get_count (data_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Set info of edge-data - invalid param.
 */
TEST(edgeData, setInfoInvalidParam01_n) {
  int ret;

  ret = nns_edge_data_set_info (NULL, "temp-key", "temp-value");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info of edge-data - invalid param.
 */
TEST(edgeData, setInfoInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_set_info (data_h, "temp-key", "temp-value");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info of edge-data - invalid param.
 */
TEST(edgeData, setInfoInvalidParam03_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, NULL, "temp-value");
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set info of edge-data - invalid param.
 */
TEST(edgeData, setInfoInvalidParam04_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "temp-key", NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get info of edge-data - invalid param.
 */
TEST(edgeData, getInfoInvalidParam01_n) {
  char *value = NULL;
  int ret;

  ret = nns_edge_data_get_info (NULL, "temp-key", &value);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get info of edge-data - invalid param.
 */
TEST(edgeData, getInfoInvalidParam02_n) {
  nns_edge_data_h data_h;
  nns_edge_data_s *ed;
  char *value = NULL;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "temp-key", "temp-value");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ed = (nns_edge_data_s *) data_h;
  ed->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_data_get_info (data_h, "temp-key", &value);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ed->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get info of edge-data - invalid param.
 */
TEST(edgeData, getInfoInvalidParam03_n) {
  nns_edge_data_h data_h;
  char *value = NULL;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "temp-key", "temp-value");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get_info (data_h, NULL, &value);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get info of edge-data - invalid param.
 */
TEST(edgeData, getInfoInvalidParam04_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "temp-key", "temp-value");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get_info (data_h, "temp-key", NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get info of edge-data - invalid param.
 */
TEST(edgeData, getInfoInvalidParam05_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "temp-key", "temp-value");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_get_info (data_h, "", NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge event - invalid param.
 */
TEST(edgeEvent, createInvalidParam01_n) {
  nns_edge_event_h event_h;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_UNKNOWN, &event_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Create edge event - invalid param.
 */
TEST(edgeEvent, createInvalidParam02_n) {
  nns_edge_event_h event_h;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Destroy edge event - invalid param.
 */
TEST(edgeEvent, destroyInvalidParam01_n) {
  int ret;

  ret = nns_edge_event_destroy (NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Destroy edge event - invalid param.
 */
TEST(edgeEvent, destroyInvalidParam02_n) {
  nns_edge_event_h event_h;
  nns_edge_event_s *ee;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_set_data (event_h, data, data_len, free);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ee = (nns_edge_event_s *) event_h;
  ee->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_event_destroy (event_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ee->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Set edge event data - invalid param.
 */
TEST(edgeEvent, setDataInvalidParam01_n) {
  nns_edge_event_h event_h;
  nns_edge_event_s *ee;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_event_set_data (NULL, data, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Set edge event data - invalid param.
 */
TEST(edgeEvent, setDataInvalidParam02_n) {
  nns_edge_event_h event_h;
  nns_edge_event_s *ee;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_set_data (event_h, NULL, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Set edge event data - invalid param.
 */
TEST(edgeEvent, setDataInvalidParam03_n) {
  nns_edge_event_h event_h;
  nns_edge_event_s *ee;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_set_data (event_h, data, 0, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Set edge event data - invalid param.
 */
TEST(edgeEvent, setDataInvalidParam04_n) {
  nns_edge_event_h event_h;
  nns_edge_event_s *ee;
  void *data;
  size_t data_len;
  int ret;

  data_len = 10U * sizeof (int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ee = (nns_edge_event_s *) event_h;
  ee->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_event_set_data (event_h, data, data_len, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ee->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  free (data);
}

/**
 * @brief Get edge event type.
 */
TEST(edgeEvent, getType) {
  nns_edge_event_h event_h;
  nns_edge_event_e event = NNS_EDGE_EVENT_UNKNOWN;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_get_type (event_h, &event);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_EQ (event, NNS_EDGE_EVENT_CUSTOM);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get edge event type - invalid param.
 */
TEST(edgeEvent, getTypeInvalidParam01_n) {
  nns_edge_event_e event;
  int ret;

  ret = nns_edge_event_get_type (NULL, &event);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get edge event type - invalid param.
 */
TEST(edgeEvent, getTypeInvalidParam02_n) {
  nns_edge_event_h event_h;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_get_type (event_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Get edge event type - invalid param.
 */
TEST(edgeEvent, getTypeInvalidParam03_n) {
  nns_edge_event_h event_h;
  nns_edge_event_e event;
  nns_edge_event_s *ee;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ee = (nns_edge_event_s *) event_h;
  ee->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_event_get_type (event_h, &event);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ee->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse new data of edge event.
 */
TEST(edgeEvent, parseNewData) {
  nns_edge_event_h event_h;
  nns_edge_data_h data_h, result_h;
  nns_edge_event_s *ee;
  void *data, *result;
  size_t data_len, result_len;
  char *result_value;
  unsigned int i, count;
  int ret;

  data_len = 10U * sizeof (unsigned int);
  data = malloc (data_len);
  ASSERT_TRUE (data != NULL);

  for (i = 0; i < 10U; i++)
    ((unsigned int *) data)[i] = i;

  ret = nns_edge_data_create (&data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_add (data_h, data, data_len, free);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_set_info (data_h, "temp-key1", "temp-data-val1");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  ret = nns_edge_data_set_info (data_h, "temp-key2", "temp-data-val2");
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_create (NNS_EDGE_EVENT_NEW_DATA_RECEIVED, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_set_data (event_h, data_h, sizeof (nns_edge_data_h), NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_parse_new_data (event_h, &result_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  /* Compare data and info */
  ret = nns_edge_data_get_count (result_h, &count);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_EQ (count, 1U);

  ret = nns_edge_data_get (result_h, 0, &result, &result_len);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  for (i = 0; i < 10U; i++)
    EXPECT_EQ (((unsigned int *) result)[i], i);

  ret = nns_edge_data_get_info (result_h, "temp-key1", &result_value);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_STREQ (result_value, "temp-data-val1");
  free (result_value);

  ret = nns_edge_data_get_info (result_h, "temp-key2", &result_value);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_STREQ (result_value, "temp-data-val2");
  free (result_value);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (data_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_data_destroy (result_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse new data of edge event - invalid param.
 */
TEST(edgeEvent, parseNewDataInvalidParam01_n) {
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_event_parse_new_data (NULL, &data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse new data of edge event - invalid param.
 */
TEST(edgeEvent, parseNewDataInvalidParam02_n) {
  nns_edge_event_h event_h;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_NEW_DATA_RECEIVED, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_parse_new_data (event_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse new data of edge event - invalid param.
 */
TEST(edgeEvent, parseNewDataInvalidParam03_n) {
  nns_edge_event_h event_h;
  nns_edge_data_h data_h;
  nns_edge_event_s *ee;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_NEW_DATA_RECEIVED, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ee = (nns_edge_event_s *) event_h;
  ee->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_event_parse_new_data (event_h, &data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ee->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse new data of edge event - invalid param.
 */
TEST(edgeEvent, parseNewDataInvalidParam04_n) {
  nns_edge_event_h event_h;
  nns_edge_data_h data_h;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_parse_new_data (event_h, &data_h);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse capability of edge event.
 */
TEST(edgeEvent, parseCapability) {
  const char capability[] = "temp-capability";
  nns_edge_event_h event_h;
  char *caps = NULL;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CAPABILITY, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_set_data (event_h, (void *) capability, strlen (capability), NULL);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_parse_capability (event_h, &caps);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
  EXPECT_STREQ (caps, capability);
  free (caps);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse capability of edge event - invalid param.
 */
TEST(edgeEvent, parseCapabilityInvalidParam01_n) {
  char *caps = NULL;
  int ret;

  ret = nns_edge_event_parse_capability (NULL, &caps);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse capability of edge event - invalid param.
 */
TEST(edgeEvent, parseCapabilityInvalidParam02_n) {
  nns_edge_event_h event_h;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CAPABILITY, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_parse_capability (event_h, NULL);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse capability of edge event - invalid param.
 */
TEST(edgeEvent, parseCapabilityInvalidParam03_n) {
  nns_edge_event_h event_h;
  nns_edge_event_s *ee;
  char *caps = NULL;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CAPABILITY, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ee = (nns_edge_event_s *) event_h;
  ee->magic = NNS_EDGE_MAGIC_DEAD;

  ret = nns_edge_event_parse_capability (event_h, &caps);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ee->magic = NNS_EDGE_MAGIC;

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Parse capability of edge event - invalid param.
 */
TEST(edgeEvent, parseCapabilityInvalidParam04_n) {
  nns_edge_event_h event_h;
  char *caps = NULL;
  int ret;

  ret = nns_edge_event_create (NNS_EDGE_EVENT_CUSTOM, &event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_parse_capability (event_h, &caps);
  EXPECT_NE (ret, NNS_EDGE_ERROR_NONE);

  ret = nns_edge_event_destroy (event_h);
  EXPECT_EQ (ret, NNS_EDGE_ERROR_NONE);
}

/**
 * @brief Main gtest
 */
int
main (int argc, char **argv)
{
  int result = -1;

  try {
    testing::InitGoogleTest (&argc, argv);
  } catch (...) {
    nns_edge_loge ("Catch exception, failed to init google test.");
  }

  try {
    result = RUN_ALL_TESTS ();
  } catch (...) {
    nns_edge_loge ("Catch exception, failed to run the unittest.");
  }

  return result;
}
