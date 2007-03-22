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

