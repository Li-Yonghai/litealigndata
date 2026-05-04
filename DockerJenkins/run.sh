#!/bin/bash
java -Djava.awt.headless=true -jar jenkins.war --httpPort=8080 >/dev/null 2>&1 & 
