#!/bin/bash

TAG="1.0"

# Updating 'master' branch
git checkout master
git tag -d ${TAG}
git push origin :refs/tags/${TAG}
git tag ${TAG}
git push origin ${TAG}

# Updating 'duo' branch
DTAG="${TAG}d"
git checkout duo
git tag -d ${DTAG}
git push origin :refs/tags/${DTAG}
git tag ${DTAG}
git push origin ${DTAG}

# Updating 'handle' branch
HTAG="${TAG}h"
git checkout handle
git tag -d ${HTAG}
git push origin :refs/tags/${HTAG}
git tag ${HTAG}
git push origin ${HTAG}

# Updating 'orbit' branch
OTAG="${TAG}o"
git checkout orbit
git tag -d ${OTAG}
git push origin :refs/tags/${OTAG}
git tag ${OTAG}
git push origin ${OTAG}

# Updating 'gloves' branch
GTAG="${TAG}g"
git checkout gloves
git tag -d ${GTAG}
git push origin :refs/tags/${GTAG}
git tag ${GTAG}
git push origin ${GTAG}

# Updating 'chromadeck' branch
GTAG="${TAG}c"
git checkout chromadeck
git tag -d ${GTAG}
git push origin :refs/tags/${GTAG}
git tag ${GTAG}
git push origin ${GTAG}

