# $LOG expected to be set

LIST="/home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf /usr/share/speech-dispatcher/conf/speechd.conf"

isAdded() {
    local filename="$1"
    local module="$2"
    grep -Eq '^[[:space:]]*AddModule[[:space:]]+"'"$module"'"[[:space:]]+"sd_'"$module"'"[[:space:]]+"'"$module"'.conf"[[:space:]]*' "$filename"
}

isAnyAdded() {
    local filename="$1"
    grep -Eq '^[[:space:]]*AddModule[[:space:]]+' "$filename"
}

addModule() {
    local filename="$1"
    local module="$2"
    sed -i -e '/^[[:space:]]*AddModule[[:space:]]/b ins' -e b -e ':ins' -e 'a\'$'\n''AddModule "'$module'"        "sd_'$module'"     "'$module'.conf"' -e ': done' -e 'n;b done' "$filename"
}

removeModule() {
    local filename="$1"
    local module="$2"
    sed -i -r '/^.*AddModule[[:space:]]+"'$module'"[[:space:]]+.*/d' "$filename"
}


isDefaultModule() {
    local filename="$1"
    local module="$2"
    grep -Eq '^[[:space:]]*DefaultModule[[:space:]]+'"$module"'[[:space:]]*' "$filename"
}

setDefaultModule() {
    local filename="$1"
    local newModule="$2"    
	local oldModule=$(grep "^[[:space:]]*DefaultModule\(.*\)" "$filename" | awk '{print $2}')
	if [ -n "$oldModule" ]; then
		sed -i "/^[[:space:]]*#[[:space:]]*voxinadded/d" "$filename"
		sed -i -e "\$a#voxinadded $oldModule" -e 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$newModule"'/' "$filename"
	else
		sed -i 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$newModule"'/' "$filename"
	fi
}

function spd_conf_unset() {
    local module="$1"
    ls -t $LIST 2>>"$LOG" | while read file; do		
		local newModule=$(grep "^[[:space:]]*#[[:space:]]*voxinadded \(.*\)" "$file" | awk '{print $2}')

		isAdded "$file" "$module" && removeModule "$file" "$module"
		
		if [ -n "$newModule" ]; then
			sed -i -e "/^[[:space:]]*#[[:space:]]*voxinadded/d" -e 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$newModule"'/' "$file"
			return
		fi
		
		local a=$(which spd-say) && newModule=$(spd-say -O | grep espeak | head -1)
		if [ -z "$newModule" ]; then
			a=$(ldconfig -p | grep libespeak)
			echo "$a" | grep libespeak-ng.so &> /dev/null
			if [ $? = 0 ]; then
				newModule=espeak-ng
				else
					echo "$a" | grep libespeak.so &> /dev/null
					[ $? = 0 ] && newModule=espeak
			fi
		fi

		if [ -n "$newModule" ]; then
			sed -i 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$newModule"'/' "$file"
		fi

	done
}

function spd_conf_set() {
    local module="$1"
    local file
    
    ls -t $LIST 2>>"$LOG" | while read file; do
	isAnyAdded "$file"
	if [ "$?" = 0 ]; then
	    isAdded "$file" "$module" || addModule "$file" "$module"
	fi
	isDefaultModule "$file" "$module"
	if [ "$?" = "1" ]; then
	    setDefaultModule "$file" "$module"
	fi
    done
}

