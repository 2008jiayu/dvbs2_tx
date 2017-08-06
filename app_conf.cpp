/*
 * Simple DVB-S2 transmitter application based on gnuradio/gr-dtv
 *
 * Copyright 2017 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "app_conf.h"

#define RF_GAIN_ID      10
#define IF_GAIN_ID      11
// A..Z is 65..90
// a..z is 97..122


struct option options[] = {
    /* simple switches with no argument */
    { "help",          no_argument,       0, 'h'},

    /* input settings */
    { "udp",           required_argument, 0, 'u'},
    { "frequency",     required_argument, 0, 'f'},

    { "rf-gain",       required_argument, 0, RF_GAIN_ID},
    { "if-gain",       required_argument, 0, IF_GAIN_ID},

    { 0, 0, 0, 0}
};

static void help(void)
{
#define PROG_NAME "dvbs2_tx"

    fprintf(stdout, "%s",
        "\n Usage: " PROG_NAME " [options]\n"
        "\n"
        "  -h        --help\n"
        "  -u        --udp\n"
        "  -f 1.2G   --frequency=1280M\n"
        "\n"
        "  --rf-gain=14\n"
        "  --if-gain=47\n"
        "\n"
        );
}

#if 0
/* Parse frequency string as single precision float. */
static float parse_freq_float(char * freq_str)
{
    size_t n = strlen(freq_str);
    double multi = 1.0;

    if (n == 0)
        return 0;

    switch (freq_str[n-1])
    {
        case 'k':
        case 'K':
            multi = 1.e3;
            break;

        case 'M':
            multi = 1.e6;
            break;

        case 'G':
            multi = 1.e9;
            break;
    }

    return (float)(multi * atof(freq_str));
}
#endif

/* Parse frequency string as uint64. */
static uint64_t parse_freq_u64(char * freq_str)
{
    size_t n = strlen(freq_str);
    double multi = 1.0;
    double res;

    if (n == 0)
        return 0;

    switch (freq_str[n-1])
    {
        case 'k':
        case 'K':
            multi = 1.e3;
            break;

        case 'M':
            multi = 1.e6;
            break;

        case 'G':
            multi = 1.e9;
            break;
    }

    res = multi * atof(freq_str);
    if (res < 0.0)
        res = 0;

    return (uint64_t)(res);
}

static void print_conf(app_conf_t * conf)
{
    fprintf(stderr, "Transmitter configuration:\n"
            "  TS input: %s\n"
            "  RF  freq: %"PRIu64" Hz\n"
            "  RF  gain: %u\n"
            "  IF  gain: %u\n",
            conf->udp_input ? "UDP" : "stdin",
            conf->rf_freq, conf->rf_gain, conf->if_gain);
}

int app_conf_init(app_conf_t * conf, int argc, char ** argv)
{
    int         option;
    int         idx;

    conf->rf_freq = 1280000000;
    conf->rf_gain = 14;
    conf->if_gain = 47;
    conf->udp_input = false;

    if (argc > 1)
    {
        while ((option = getopt_long(argc, argv, "c:f:uh", options, &idx)) != -1)
        {
            switch (option)
            {
            case 'f':
                conf->rf_freq = parse_freq_u64(optarg);
                break;
            case 'u':
                conf->udp_input = true;
                break;
            case RF_GAIN_ID:
                conf->rf_gain = atoi(optarg);
                break;
            case IF_GAIN_ID:
                conf->if_gain = atoi(optarg);
                break;
            case 'h':
            default:
                help();
                return APP_CONF_ERROR;
            }
        }
    }
    print_conf(conf);

    return APP_CONF_OK;
}
