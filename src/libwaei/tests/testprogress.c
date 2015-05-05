#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libwaei/libwaei.h>

struct _Fixture {
  LwProgress * progress;
  gboolean progress_changed;
  gdouble total;
  gdouble current;
  gdouble fraction;
  gboolean completed;
  gchar const * PRIMARY_MESSAGE;
  gchar const * SECONDARY_MESSAGE;
  gchar const * STEP_MESSAGE;
};
typedef struct _Fixture Fixture;


void
assert_fixture_equal (Fixture * self,
                      Fixture * other)
{
    g_assert_cmpint (self->progress_changed, ==, other->progress_changed);
    g_assert_cmpfloat (self->current, ==, other->current);
    g_assert_cmpfloat (self->total, ==, other->total);
    g_assert_cmpfloat (self->fraction, ==, other->fraction);
    g_assert_cmpfloat (self->completed, ==, other->completed);
}


static void
progress_changed (LwProgress * self,
                  Fixture    * fixture)
{
    fixture->progress_changed = TRUE;
    fixture->total = lw_progress_get_total (self);
    fixture->current = lw_progress_get_current (self);
    fixture->fraction = lw_progress_get_fraction (self);
    fixture->completed = lw_progress_completed (self);
}

static void
primary_message_notify (LwProgress * self,
                        GParamSpec * pspec,
                        Fixture    * fixture)
{
    fixture->PRIMARY_MESSAGE = lw_progress_get_primary_message (self);
}

static void
secondary_message_notify (LwProgress * self,
                          GParamSpec * pspec,
                          Fixture    * fixture)
{
    fixture->SECONDARY_MESSAGE = lw_progress_get_secondary_message (self);
}

static void
step_message_notify (LwProgress * self,
                          GParamSpec * pspec,
                          Fixture    * fixture)
{
    fixture->STEP_MESSAGE = lw_progress_get_step_message (self);
}


void setup (Fixture *fixture, gconstpointer data)
{
  fixture->progress = lw_progress_new ();
  g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (progress_changed), fixture);
  g_signal_connect (fixture->progress, "notify::primary-message", G_CALLBACK (primary_message_notify), fixture);
  g_signal_connect (fixture->progress, "notify::secondary-message", G_CALLBACK (secondary_message_notify), fixture);
  g_signal_connect (fixture->progress, "notify::step-message", G_CALLBACK (step_message_notify), fixture);
}


void teardown (Fixture *fixture, gconstpointer data)
{
  g_object_unref (fixture->progress);
  fixture->progress = NULL;
}


void
set_current_less_than_total (Fixture       * fixture,
                             gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, 10);

    //Assert
    g_assert_cmpfloat (lw_progress_get_current (progress), ==, 10.0);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 100.0,
      .fraction = 0.1,
      .current = 10.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_current_more_than_total (Fixture       * fixture,
                             gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, 110);

    //Assert
    g_assert_cmpfloat (lw_progress_get_current (progress), ==, 100.0);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 100.0,
      .fraction = 1.0,
      .current = 100.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}

void
set_current_to_negative (Fixture       * fixture,
                         gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, -10);

    //Assert
    g_assert_cmpfloat (lw_progress_get_current (progress), ==, 0.0);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 100.0,
      .fraction = 0.0,
      .current = 0.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_current_without_total (Fixture       * fixture,
                                       gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_current (progress, 10);

    //Assert
    g_assert_cmpfloat (lw_progress_get_current (progress), ==, 0.0);
    Fixture expected_fixture = {
      .progress_changed = FALSE,
      .total = 0.0,
      .fraction = 0.0,
      .current = 0.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_current_with_same_value (Fixture       * fixture,
                             gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, 10);
    fixture->progress_changed = FALSE;
    lw_progress_set_current (progress, 10);

    //Assert
    g_assert_cmpfloat (lw_progress_get_current (progress), ==, 10.0);
    Fixture expected_fixture = {
      .progress_changed = FALSE,
      .total = 100.0,
      .fraction = 0.1,
      .current = 10.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_total_with_negative_value (Fixture       * fixture,
                               gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, -100);

    //Assert
    g_assert_cmpfloat (lw_progress_get_total (progress), ==, 0.0);
    Fixture expected_fixture = {
      .progress_changed = FALSE,
      .total = 0.0,
      .fraction = 0.0,
      .current = 0.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_total_after_current_with_larger_value (Fixture       * fixture,
                                           gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, 10);
    fixture->progress_changed = FALSE;
    lw_progress_set_total (progress, 200);

    //Assert
    g_assert_cmpfloat (lw_progress_get_total (progress), ==, 200.0);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 200.0,
      .fraction = 0.05,
      .current = 10.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_completed_true_with_no_setup (Fixture       * fixture,
                                  gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_completed (progress, TRUE);

    //Assert
    g_assert_cmpfloat (lw_progress_get_total (progress), ==, 0.0);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 0.0,
      .fraction = 1.0,
      .current = 0.0,
      .completed = TRUE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_completed_false_with_no_setup (Fixture       * fixture,
                                   gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_completed (progress, TRUE);
    fixture->progress_changed = FALSE;
    lw_progress_set_completed (progress, FALSE);

    //Assert
    g_assert_cmpfloat (lw_progress_get_total (progress), ==, 0.0);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 0.0,
      .fraction = 0.0,
      .current = 0.0,
      .completed = FALSE
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_completed_twice_with_no_setup (Fixture       * fixture,
                                   gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_completed (progress, TRUE);
    fixture->progress_changed = FALSE;
    lw_progress_set_completed (progress, TRUE);

    //Assert
    g_assert_cmpfloat (lw_progress_get_total (progress), ==, 0.0);
    Fixture expected_fixture = {
      .progress_changed = FALSE,
      .total = 0.0,
      .fraction = 1.0,
      .current = 0.0,
      .completed = TRUE 
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_completed_true (Fixture       * fixture,
                    gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, 10);
    fixture->progress_changed = FALSE;
    lw_progress_set_completed (progress, TRUE);

    //Assert
    g_assert_cmpint (lw_progress_completed (progress), ==, TRUE);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 100.0,
      .fraction = 1.0,
      .current = 10.0,
      .completed = TRUE 
    };
    assert_fixture_equal (fixture, &expected_fixture);
}

void
set_completed_true_then_false (Fixture       * fixture,
                               gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_total (progress, 100);
    lw_progress_set_current (progress, 10);
    lw_progress_set_completed (progress, TRUE);
    fixture->progress_changed = FALSE;
    lw_progress_set_completed (progress, FALSE);

    //Assert
    g_assert_cmpint (lw_progress_completed (progress), ==, FALSE);
    Fixture expected_fixture = {
      .progress_changed = TRUE,
      .total = 100.0,
      .fraction = 0.1,
      .current = 10.0,
      .completed = FALSE 
    };
    assert_fixture_equal (fixture, &expected_fixture);
}


void
set_error_new (Fixture       * fixture,
               gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    GError * error = NULL;

    //Act
    g_set_error_literal (
      &error,
      g_quark_from_static_string ("test-domain"),
      2, 
      "test error"
    );
    lw_progress_set_error (progress, error);

    //Assert
    g_assert_error (
      lw_progress_get_error (progress),
      g_quark_from_static_string ("test-domain"),
      2
    );
    g_assert (lw_progress_get_error (progress) != error);
    g_assert_true (lw_progress_should_abort (progress));
    g_assert_true (lw_progress_errored (progress));
    g_assert_false (lw_progress_is_cancelled (progress));

    g_clear_error (&error);
    error = NULL;
}


void
set_error_new_then_null (Fixture       * fixture,
                         gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    GError * error = NULL;

    //Act
    g_set_error_literal (
      &error,
      g_quark_from_static_string ("test-domain"),
      2, 
      "test error"
    );
    lw_progress_set_error (progress, error);
    lw_progress_set_error (progress, NULL);

    //Assert
    g_assert_null (lw_progress_get_error (progress));
    g_assert_false (lw_progress_should_abort (progress));
    g_assert_false (lw_progress_errored (progress));
    g_assert_false (lw_progress_is_cancelled (progress));

    g_clear_error (&error);
    error = NULL;
}



void
take_error_new (Fixture       * fixture,
                gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    GError * error = NULL;

    //Act
    g_set_error_literal (
      &error,
      g_quark_from_static_string ("test-domain"),
      2, 
      "test error"
    );
    lw_progress_take_error (progress, error);
    error = NULL;

    //Assert
    g_assert_error (
      lw_progress_get_error (progress),
      g_quark_from_static_string ("test-domain"),
      2
    );
    g_assert (lw_progress_get_error (progress) != error);
    g_assert_true (lw_progress_should_abort (progress));
    g_assert_true (lw_progress_errored (progress));
    g_assert_false (lw_progress_is_cancelled (progress));

}


void
take_error_new_then_null (Fixture       * fixture,
                          gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;
    GError * error = NULL;

    //Act
    g_set_error_literal (
      &error,
      g_quark_from_static_string ("test-domain"),
      2, 
      "test error"
    );
    lw_progress_take_error (progress, error);
    error = NULL;
    lw_progress_take_error (progress, NULL);

    //Assert
    g_assert_null (lw_progress_get_error (progress));
    g_assert_false (lw_progress_should_abort (progress));
    g_assert_false (lw_progress_errored (progress));
    g_assert_false (lw_progress_is_cancelled (progress));
}


void
cancel_when_not_cancelled (Fixture       * fixture,
        gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_cancel (progress);

    //Assert
    g_assert_true (lw_progress_should_abort (progress));
    g_assert_false (lw_progress_errored (progress));
    g_assert_true (lw_progress_is_cancelled (progress));

}


void
cancel_twice (Fixture       * fixture,
              gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_cancel (progress);
    lw_progress_cancel (progress);

    //Assert
    g_assert_true (lw_progress_should_abort (progress));
    g_assert_false (lw_progress_errored (progress));
    g_assert_true (lw_progress_is_cancelled (progress));

}


void
set_primary_message_new (Fixture       * fixture,
                         gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_primary_message (progress, "test primary message");

    //Assert
    g_assert_cmpstr (lw_progress_get_primary_message (progress), ==, "test primary message");
    g_assert_cmpstr (fixture->PRIMARY_MESSAGE, ==, "test primary message");

}


void
set_primary_message_null (Fixture       * fixture,
                          gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_primary_message (progress, NULL);

    //Assert
    g_assert_cmpstr (lw_progress_get_primary_message (progress), ==, "");
    g_assert_cmpstr (fixture->PRIMARY_MESSAGE, ==, "");

}

void
set_primary_message_twice (Fixture       * fixture,
                          gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_primary_message (progress, "test primary message");
    lw_progress_set_primary_message (progress, "test primary message 2");

    //Assert
    g_assert_cmpstr (lw_progress_get_primary_message (progress), ==, "test primary message 2");
    g_assert_cmpstr (fixture->PRIMARY_MESSAGE, ==, "test primary message 2");
}



void
set_primary_message_printf_new (Fixture       * fixture,
                                gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_primary_message_printf (progress, "test primary message %s", "printf");

    //Assert
    g_assert_cmpstr (lw_progress_get_primary_message (progress), ==, "test primary message printf");
    g_assert_cmpstr (fixture->PRIMARY_MESSAGE, ==, "test primary message printf");

}


void
set_secondary_message_new (Fixture       * fixture,
                           gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_secondary_message (progress, "test secondary message");

    //Assert
    g_assert_cmpstr (lw_progress_get_secondary_message (progress), ==, "test secondary message");
    g_assert_cmpstr (fixture->SECONDARY_MESSAGE, ==, "test secondary message");

}


void
set_secondary_message_null (Fixture       * fixture,
                            gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_secondary_message (progress, NULL);

    //Assert
    g_assert_cmpstr (lw_progress_get_secondary_message (progress), ==, "");
    g_assert_cmpstr (fixture->SECONDARY_MESSAGE, ==, "");

}

void
set_secondary_message_twice (Fixture       * fixture,
                             gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_secondary_message (progress, "test secondary message");
    lw_progress_set_secondary_message (progress, "test secondary message 2");

    //Assert
    g_assert_cmpstr (lw_progress_get_secondary_message (progress), ==, "test secondary message 2");
    g_assert_cmpstr (fixture->SECONDARY_MESSAGE, ==, "test secondary message 2");
}


void
set_secondary_message_printf_new (Fixture       * fixture,
                                  gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_secondary_message_printf (progress, "test secondary message %s", "printf");

    //Assert
    g_assert_cmpstr (lw_progress_get_secondary_message (progress), ==, "test secondary message printf");
    g_assert_cmpstr (fixture->SECONDARY_MESSAGE, ==, "test secondary message printf");
}


void
set_step_message_new (Fixture       * fixture,
                      gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_step_message (progress, "test step message");

    //Assert
    g_assert_cmpstr (lw_progress_get_step_message (progress), ==, "test step message");
    g_assert_cmpstr (fixture->STEP_MESSAGE, ==, "test step message");

}


void
set_step_message_null (Fixture       * fixture,
                       gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_step_message (progress, NULL);

    //Assert
    g_assert_cmpstr (lw_progress_get_step_message (progress), ==, "");
    g_assert_cmpstr (fixture->STEP_MESSAGE, ==, "");

}

void
set_step_message_twice (Fixture       * fixture,
                        gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_step_message (progress, "test step message");
    lw_progress_set_step_message (progress, "test step message 2");

    //Assert
    g_assert_cmpstr (lw_progress_get_step_message (progress), ==, "test step message 2");
    g_assert_cmpstr (fixture->STEP_MESSAGE, ==, "test step message 2");
}


void
set_step_message_printf_new (Fixture       * fixture,
                             gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_step_message_printf (progress, "test step message %s", "printf");

    //Assert
    g_assert_cmpstr (lw_progress_get_step_message (progress), ==, "test step message printf");
    g_assert_cmpstr (fixture->STEP_MESSAGE, ==, "test step message printf");
}


void
set_prefered_chunk_size_without_chunk_size (Fixture       * fixture,
                                            gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_prefered_chunk_size (progress, 3);

    //Assert
    g_assert_cmpuint (lw_progress_get_prefered_chunk_size (progress), ==, 3);
    g_assert_cmpuint (lw_progress_get_chunk_size (progress), ==, 3);
}


void
set_prefered_chunk_size_with_chunk_size (Fixture       * fixture,
                                         gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_prefered_chunk_size (progress, 3);
    lw_progress_set_chunk_size (progress, 5);

    //Assert
    g_assert_cmpuint (lw_progress_get_prefered_chunk_size (progress), ==, 3);
    g_assert_cmpuint (lw_progress_get_chunk_size (progress), ==, 3);
}


void
set_prefered_chunk_unset (Fixture       * fixture,
                          gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act

    //Assert
    g_assert_cmpuint (lw_progress_get_prefered_chunk_size (progress), ==, 0);
    g_assert_cmpuint (lw_progress_get_chunk_size (progress), ==, lw_io_get_pagesize());
}


void
set_chunk_when_prefered_chunk_is_set (Fixture       * fixture,
                                      gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_prefered_chunk_size (progress, 1);
    lw_progress_set_chunk_size (progress, 2);

    //Assert
    g_assert_cmpuint (lw_progress_get_prefered_chunk_size (progress), ==, 1);
    g_assert_cmpuint (lw_progress_get_chunk_size (progress), ==, 1);
}


void
set_chunk_when_prefered_chunk_is_unset (Fixture       * fixture,
                                        gconstpointer   data)
{
    //Arrange
    LwProgress * progress = fixture->progress;

    //Act
    lw_progress_set_chunk_size (progress, 2);

    //Assert
    g_assert_cmpuint (lw_progress_get_prefered_chunk_size (progress), ==, 0);
    g_assert_cmpuint (lw_progress_get_chunk_size (progress), ==, 2);
}


gint
main (gint argc, gchar *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add ("/set_current/less_than_total", Fixture, NULL, setup, set_current_less_than_total, teardown);
    g_test_add ("/set_current/more_than_total", Fixture, NULL, setup, set_current_more_than_total, teardown);
    g_test_add ("/set_current/to_negative", Fixture, NULL, setup, set_current_to_negative, teardown);
    g_test_add ("/set_current/without_total", Fixture, NULL, setup, set_current_without_total, teardown);
    g_test_add ("/set_current/with_same_value", Fixture, NULL, setup, set_current_with_same_value, teardown);

    g_test_add ("/set_total/with_negative_value", Fixture, NULL, setup, set_total_with_negative_value, teardown);
    g_test_add ("/set_total/after_current_with_larger_value", Fixture, NULL, setup, set_total_after_current_with_larger_value, teardown);

    g_test_add ("/set_completed/true_with_no_setup", Fixture, NULL, setup, set_completed_true_with_no_setup, teardown);
    g_test_add ("/set_completed/false_with_no_setup", Fixture, NULL, setup, set_completed_false_with_no_setup, teardown);
    g_test_add ("/set_completed/twice_with_no_setup", Fixture, NULL, setup, set_completed_twice_with_no_setup, teardown);
    g_test_add ("/set_completed/true", Fixture, NULL, setup, set_completed_true, teardown);
    g_test_add ("/set_completed/true_then_false", Fixture, NULL, setup, set_completed_true_then_false, teardown);

    g_test_add ("/set_error/new", Fixture, NULL, setup, set_error_new, teardown);
    g_test_add ("/set_error/new_then_null", Fixture, NULL, setup, set_error_new_then_null, teardown);
    g_test_add ("/take_error/new", Fixture, NULL, setup, take_error_new, teardown);
    g_test_add ("/take_error/new_then_null", Fixture, NULL, setup, take_error_new_then_null, teardown);

    g_test_add ("/cancel/when_not_cancelled", Fixture, NULL, setup, cancel_when_not_cancelled, teardown);
    g_test_add ("/cancel/twice", Fixture, NULL, setup, cancel_twice, teardown);

    g_test_add ("/set_primary_message/new", Fixture, NULL, setup, set_primary_message_new, teardown);
    g_test_add ("/set_primary_message/null", Fixture, NULL, setup, set_primary_message_null, teardown);
    g_test_add ("/set_primary_message/twice", Fixture, NULL, setup, set_primary_message_twice, teardown);

    g_test_add ("/set_primary_message_printf/new", Fixture, NULL, setup, set_primary_message_printf_new, teardown);

    g_test_add ("/set_secondary_message/new", Fixture, NULL, setup, set_secondary_message_new, teardown);
    g_test_add ("/set_secondary_message/null", Fixture, NULL, setup, set_secondary_message_null, teardown);
    g_test_add ("/set_secondary_message/twice", Fixture, NULL, setup, set_secondary_message_twice, teardown);

    g_test_add ("/set_secondary_message_printf/new", Fixture, NULL, setup, set_secondary_message_printf_new, teardown);

    g_test_add ("/set_step_message/new", Fixture, NULL, setup, set_step_message_new, teardown);
    g_test_add ("/set_step_message/null", Fixture, NULL, setup, set_step_message_null, teardown);
    g_test_add ("/set_step_message/twice", Fixture, NULL, setup, set_step_message_twice, teardown);

    g_test_add ("/set_step_message_printf/new", Fixture, NULL, setup, set_step_message_printf_new, teardown);

    g_test_add ("/set_prefered_chunk_size/without_chunk_size", Fixture, NULL, setup, set_prefered_chunk_size_without_chunk_size, teardown);
    g_test_add ("/set_prefered_chunk_size/with_chunk_size", Fixture, NULL, setup, set_prefered_chunk_size_with_chunk_size, teardown);
    g_test_add ("/set_prefered_chunk_size/unset", Fixture, NULL, setup, set_prefered_chunk_unset, teardown);

    g_test_add ("/set_chunk_size/when_prefered_chunk_is_set", Fixture, NULL, setup, set_chunk_when_prefered_chunk_is_set, teardown);
    g_test_add ("/set_chunk_size/when_prefered_chunk_is_unset", Fixture, NULL, setup, set_chunk_when_prefered_chunk_is_unset, teardown);

    return g_test_run();
}



