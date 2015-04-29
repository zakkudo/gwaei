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
}


void
progress_changed (LwProgress * self,
                  Fixture    * fixture)
{
    fixture->progress_changed = TRUE;
    fixture->total = lw_progress_get_total (self);
    fixture->current = lw_progress_get_current (self);
    fixture->fraction = lw_progress_get_fraction (self);
}


void setup (Fixture *fixture, gconstpointer data)
{
  fixture->progress = lw_progress_new ();
  g_signal_connect (fixture->progress, "progress-changed", G_CALLBACK (progress_changed), fixture);
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
      .current = 10.0
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
      .current = 100.0
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
      .current = 0.0
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
      .progress_changed = TRUE,
      .total = 0.0,
      .fraction = 0.0,
      .current = 0.0
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
      .current = 10.0
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
      .current = 0.0
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
      .current = 10.0
    };
    assert_fixture_equal (fixture, &expected_fixture);
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

    return g_test_run();
}



