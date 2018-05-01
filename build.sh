#!/bin/sh
set -ex

go install github.com/mat007/pony/cmd/pony
go generate
cd bin && go build ..
