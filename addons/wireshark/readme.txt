To use AMQP wireshark dissector you have to:

1. Download wireshark codebase
2. Add code from

   https://svn.imatix.net/imatix/openamq/trunk/addons/wireshark

   to wireshark/plugin/amqp directory.

3. Generate c file. "gsl -script:dissector_gen.gsl amqp0-8.xml"
3. do "make install" in main wireshark directory.
4. run it as root "sudo wireshark"
5. Set capure filter to capture only AMQP packets. Set "Filter:" text
box on the main screen to "amqp".
6. Start capturing the packets. "Capture/Start".
7. Generate some AMQP traffic.
8. Click through the packets displayed.

> Possibly there are build steps missing. Mato, can you comment?

Yes, 1a)  "Do all the magic stuff required to get the rest of the build
system to know about the plugin".

If you want an answer from me now on what the steps were (I don't
remember), do the following:

1)  On your notebook in ~/wireshark/, rename the wireshark-X.YY
directory (the one where you built it) to wireshark-X.YY.new:

  mv wireshark-X.YY wireshark-X.YY.new

2)  Unpack the original tarball again, this will give you a new pristine
wireshark-X.YY directory:

  tar -xzf wireshark-X.YY.tar.gz

3)  Run a diff of the two directories:

  diff -urn wireshark-X.YY wireshark-X.YY.new > wireshark.diff

4)  Send me the result, and I'll munge it into what you need.

-mato


Yes, 1a)  "Do all the magic stuff required to get the rest of the build
system to know about the plugin