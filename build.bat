go install github.com/mat007/pony/cmd/pony
go generate
pushd bin & go build .. & popd
