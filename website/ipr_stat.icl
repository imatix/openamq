<class
    name    = "ipr_stat"
    comment = "Statistics module"
    version = "1.0"
    script  = "icl_gen"
    >
<doc>
    The statistics module is designed to capture information (raw integers)
    and process these into useful information.  The sample size is arbitrarily
    large.  Samples are double-precision floating point.
</doc>
<inherit class = "icl_object">
    <option name = "alloc" value = "cache" />
</inherit>

<context>
    double
        sum,                            //  Sum of all values
        sum2,                           //  Running deviation sum
        min,                            //  Minimum value
        max,                            //  Maximum value
        mean;                           //  Current mean
    size_t      
        count;                          //  Sample size
</context>

<method name = "new">
    <doc>
    Initialises a new statistics set.
    </doc>
</method>

<method name = "record" template = "function">
    <doc>
    Records a new sample.  The standard deviation is calculated using Knuth's
    algorithm, from http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance:

        double mean = 0;
        double S = 0;
        long N;
        for N = 1 to data.length
            double delta = data[N] - mean;
            mean = mean + delta / N;
            S = S + delta * ( data[N] - mean ); 
        end for
        return S / ( N - 1 ) // the variance

    </doc>
    <argument name = "value" type = "double">Sample value</argument>
    <local>
    double
        delta;
    </local>
    //
    self->sum += value;
    self->min  = self->count? min (value, self->min): value;
    self->max  = max (value, self->max);
    self->count++;

    delta = value - self->mean;
    self->mean += delta / self->count;
    self->sum2 += delta * (value - self->mean);
</method>

<method name = "min" template = "statfct">
    <doc>
    Returns the minimum sample value.  If the sample size is zero,
    returns zero.
    </doc>
    //
    rc = self->min;
</method>

<method name = "max" template = "statfct">
    <doc>
    Returns the maximum sample value.
    </doc>
    //
    rc = self->max;
</method>

<method name = "count" template = "function">
    <doc>
    Returns the number of samples.
    </doc>
    //
    rc = self->count;
</method>

<method name = "sum" template = "statfct">
    <doc>
    Returns the sum of all samples.
    </doc>
    //
    rc = self->sum;
</method>

<method name = "mean" template = "statfct">
    <doc>
    Returns the mean of the sample set.
    </doc>
    //
    rc = self->mean;
</method>

<method name = "var" template = "statfct">
    <doc>
    Returns the variance of the sample set.
    </doc>
    //
    rc = self->sum2 / (self->count - 1);
</method>

<method name = "dev" template = "statfct">
    <doc>
    Returns the standard deviation of the sample set.
    </doc>
    //
    rc = sqrt (self->sum2 / (self->count - 1));
</method>

<method name = "statfct" return = "rc" abstract = "1">
    <argument name = "self" type = "$(selftype) *">Reference to object</argument>
    <declare name = "rc" type = "double" default = "0">Return code</declare>
    <header>
    $(selfname:upper)_ASSERT_SANE (self);
    </header>
</method>

<method name = "selftest">
    <local>
    ipr_stat_t
        *stat;
    </local>
    stat = ipr_stat_new ();

    ipr_stat_record (stat, 5);
    ipr_stat_record (stat, 7);
    ipr_stat_record (stat, 8);
    ipr_stat_record (stat, 10);
    ipr_stat_record (stat, 10);

    assert (ipr_stat_count (stat) == 5);
    assert (ipr_stat_sum   (stat) == 40);
    assert (ipr_stat_min   (stat) == 5);
    assert (ipr_stat_max   (stat) == 10);
    assert (ipr_stat_mean  (stat) == 8);
    assert (ipr_stat_var   (stat) == 4.5);
    assert (ipr_stat_dev   (stat) > 2.12);
    assert (ipr_stat_dev   (stat) < 2.13);
    
    ipr_stat_destroy (&stat);
</method>

</class>
