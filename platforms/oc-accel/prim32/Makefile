#
# Copyright 2017 International Business Machines
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Make sure Fletcher environment variable is set.
ifndef FLETCHER_DIR
$(error FLETCHER_DIR is not set)
endif

subdirs += hw sw

all: $(subdirs)

.PHONY: $(subdirs) softlinks

# Only build if the subdirectory is existent and if Makefile is there
$(subdirs): softlinks
	@if [ -d $@ -a -f $@/Makefile ]; then			\
		$(MAKE) -C $@ || exit 1;			\
	else							\
		echo "INFO: No Makefile available in $@ ...";	\
	fi

# Cleanup for all subdirectories.
# Only dive into subdirectory if existent and if Makefile is there.
clean:
	rm -f ./hw/fletcher
	rm -f ./hw/fast-p2a
	rm -f ./hw/prim32
	@for dir in $(subdirs); do	\
		if [ -d $$dir -a -f $$dir/Makefile ]; then	\
			$(MAKE) -C $$dir $@ || exit 1;		\
		fi						\
	done
	@find . -depth -name '*~'  -exec rm -rf '{}' \; -print
	@find . -depth -name '.#*' -exec rm -rf '{}' \; -print

softlinks:
	echo "INFO: Creating Fletcher hardware symlinks."
	rm -f ./hw/fletcher
	rm -f ./hw/fast-p2a
	rm -f ./hw/prim32
	ln -fs $(FLETCHER_DIR)/hardware ./hw/fletcher
	ln -fs ../../../../hardware/vhdl ./hw/fast-p2a
	ln -fs ../../../../examples/prim32/hardware ./hw/prim32

