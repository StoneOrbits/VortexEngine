#!/bin/bash

TAG="1.1"

# Updating 'master' branch
git checkout master
git tag ${TAG}
git push origin ${TAG}

# Updating 'duo' branch
DTAG="${TAG}d"
git checkout duo
git tag ${DTAG}
git push origin ${DTAG}

# Updating 'handle' branch
HTAG="${TAG}h"
git checkout handle
git tag ${HTAG}
git push origin ${HTAG}

# Updating 'orbit' branch
OTAG="${TAG}o"
git checkout orbit
git tag ${OTAG}
git push origin ${OTAG}

# Updating 'gloves' branch
GTAG="${TAG}g"
git checkout gloves
git tag ${GTAG}
git push origin ${GTAG}

