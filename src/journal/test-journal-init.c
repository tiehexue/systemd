/* SPDX-License-Identifier: LGPL-2.1+ */
/***
  This file is part of systemd.

  Copyright 2013 Zbigniew Jędrzejewski-Szmek
***/

#include "sd-journal.h"

#include "log.h"
#include "parse-util.h"
#include "rm-rf.h"
#include "util.h"

int main(int argc, char *argv[]) {
        sd_journal *j;
        int r, i, I = 100;
        char t[] = "/tmp/journal-stream-XXXXXX";

        log_set_max_level(LOG_DEBUG);

        if (argc >= 2) {
                r = safe_atoi(argv[1], &I);
                if (r < 0)
                        log_info("Could not parse loop count argument. Using default.");
        }

        log_info("Running %d loops", I);

        assert_se(mkdtemp(t));

        for (i = 0; i < I; i++) {
                r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
                assert_se(r == 0);

                sd_journal_close(j);

                r = sd_journal_open_directory(&j, t, 0);
                assert_se(r == 0);

                sd_journal_close(j);

                j = NULL;
                r = sd_journal_open_directory(&j, t, SD_JOURNAL_LOCAL_ONLY);
                assert_se(r == -EINVAL);
                assert_se(j == NULL);
        }

        assert_se(rm_rf(t, REMOVE_ROOT|REMOVE_PHYSICAL) >= 0);

        return 0;
}
